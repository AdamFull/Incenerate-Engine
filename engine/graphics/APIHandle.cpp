#include "APIHandle.h"

#include "filesystem/vfs_helper.h"
#include "Engine.h"

#include "image/Image2D.h"
#include "image/Image3D.h"
#include "image/ImageCubemap.h"

#include "window/SDL2WindowAdapter.h"

#include "DebugDraw.h"

#include <SessionStorage.hpp>

#include "APICompatibility.h"

#include <glm/gtc/type_ptr.hpp>

#include <Helpers.h>

using namespace engine::graphics;
using namespace engine::filesystem;
using namespace engine::ecs;

CAPIHandle::CAPIHandle()
{
}

CAPIHandle::~CAPIHandle()
{
    
}

void CAPIHandle::create(const FEngineCreateInfo& createInfo)
{
	eAPI = createInfo.eAPI;

    m_pWindow = std::make_unique<CSDL2WindowAdapter>(m_pEvents);
    m_pWindow->create(createInfo.window);

    m_pWindow->getWindowSize(const_cast<int32_t*>(&createInfo.window.actualWidth), const_cast<int32_t*>(&createInfo.window.actualHeight));

    m_pFrameDoneEvent = m_pEvents->makeEvent(Events::Graphics::AllFramesDone);
    m_pReCreateEvent = m_pEvents->makeEvent(Events::Graphics::ReCreate);
    m_pViewportReCreateEvent = m_pEvents->makeEvent(Events::Graphics::ViewportReCreate);

    m_pImageManager = std::make_unique<CObjectManager<CImage>>();
    m_pShaderManager = std::make_unique<CObjectManager<CShaderObject>>();
    m_pMaterialManager = std::make_unique<CObjectManager<CMaterial>>();
    m_pVertexBufferManager = std::make_unique<CObjectManager<CVertexBufferObject>>();
    m_pRenderStageManager = std::make_unique<CObjectManager<CRenderStage>>();

	m_pDevice = std::make_unique<CDevice>(this);
	m_pDevice->create(createInfo, m_pWindow.get());

    m_pShaderLoader = std::make_unique<CShaderLoader>(m_pDevice.get(), m_pVFS);
    m_pShaderLoader->create();

    m_pImageLoader = std::make_unique<CImageLoader>(m_pVFS);
    
    m_pCommandBuffers = std::make_unique<CCommandBuffer>(m_pDevice.get());
    m_pCommandBuffers->create(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, m_pDevice->getFramesInFlight());

    m_pDebugDraw = std::make_unique<CDebugDraw>(this);
    m_pQueryPool = std::make_unique<CQueryPool>(m_pDevice.get());

    if (APICompatibility::bindlessSupport)
    {
        m_pBindlessTextures = std::make_unique<CBindlessDescriptor>(m_pDevice.get());
        m_pBindlessTextures->create();
    }

    auto empty_image_2d = std::make_unique<CImage2D>(m_pDevice.get());
    empty_image_2d->create(vk::Extent2D{ 1u, 1u }, vk::Format::eR8G8B8A8Srgb);
    auto empty_image = addImage("empty_image_2d", std::move(empty_image_2d));

    auto depth_format = m_pDevice->getDepthFormat();
    
    {
        m_mStageInfos["cascade_shadow"].srName = "Cascade shadow pass";
        m_mStageInfos["cascade_shadow"].viewport.offset = vk::Offset2D(0, 0);
        m_mStageInfos["cascade_shadow"].viewport.extent = vk::Extent2D(CASCADE_SHADOW_MAP_RESOLUTION, CASCADE_SHADOW_MAP_RESOLUTION);
        m_mStageInfos["cascade_shadow"].bIgnoreRecreation = true;
        m_mStageInfos["cascade_shadow"].bFlipViewport = false;
        m_mStageInfos["cascade_shadow"].vImages.emplace_back("cascade_shadowmap_tex", depth_format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, vk::SamplerAddressMode::eClampToEdge, EImageType::eArray2D, CASCADE_SHADOW_MAP_CASCADE_COUNT);
        m_mStageInfos["cascade_shadow"].vDescriptions.emplace_back("cascade_shadowmap_tex");
        m_mStageInfos["cascade_shadow"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eBottomOfPipe,
                    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite
                ),
                FCIDependencyDesc(
                    0,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
                    vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite
                )
            )
        );
        m_mStageInfos["cascade_shadow"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    0,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
                    vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite
                ),
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eFragmentShader,
                    vk::AccessFlagBits::eShaderRead
                )
            )
        );

        auto stageId = addRenderStage("cascade_shadow");
        auto& pStage = getRenderStage(stageId);
        pStage->create(m_mStageInfos["cascade_shadow"]);
    }

    {
        m_mStageInfos["direct_shadow"].srName = "Directional shadow pass";
        m_mStageInfos["direct_shadow"].viewport.offset = vk::Offset2D(0, 0);
        m_mStageInfos["direct_shadow"].viewport.extent = vk::Extent2D(SPOT_LIGHT_SHADOW_MAP_RESOLUTION, SPOT_LIGHT_SHADOW_MAP_RESOLUTION);
        m_mStageInfos["direct_shadow"].bIgnoreRecreation = true;
        m_mStageInfos["direct_shadow"].bFlipViewport = false;
        m_mStageInfos["direct_shadow"].vImages.emplace_back("direct_shadowmap_tex", depth_format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, vk::SamplerAddressMode::eClampToEdge, EImageType::eArray2D, MAX_SPOT_LIGHT_SHADOW_COUNT);
        m_mStageInfos["direct_shadow"].vDescriptions.emplace_back("direct_shadowmap_tex");
        m_mStageInfos["direct_shadow"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eBottomOfPipe,
                    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite
                ),
                FCIDependencyDesc(
                    0,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
                    vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite
                )
            )
        );
        m_mStageInfos["direct_shadow"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    0,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
                    vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite
                ),
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eFragmentShader,
                    vk::AccessFlagBits::eShaderRead
                )
            )
        );

        auto stageId = addRenderStage("direct_shadow");
        auto& pStage = getRenderStage(stageId);
        pStage->create(m_mStageInfos["direct_shadow"]);
    }

    {
        m_mStageInfos["omni_shadow"].srName = "Omni shadow pass";
        m_mStageInfos["omni_shadow"].viewport.offset = vk::Offset2D(0, 0);
        m_mStageInfos["omni_shadow"].viewport.extent = vk::Extent2D(POINT_LIGHT_SHADOW_MAP_RESOLUTION, POINT_LIGHT_SHADOW_MAP_RESOLUTION);
        m_mStageInfos["omni_shadow"].bIgnoreRecreation = true;
        m_mStageInfos["omni_shadow"].bFlipViewport = false;
        m_mStageInfos["omni_shadow"].vImages.emplace_back("omni_shadowmap_tex", depth_format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, vk::SamplerAddressMode::eClampToEdge, EImageType::eArrayCube, MAX_POINT_LIGHT_SHADOW_COUNT);
        m_mStageInfos["omni_shadow"].vDescriptions.emplace_back("omni_shadowmap_tex");
        m_mStageInfos["omni_shadow"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eBottomOfPipe,
                    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite
                ),
                FCIDependencyDesc(
                    0,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
                    vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite
                )
            )
        );
        m_mStageInfos["omni_shadow"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    0,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
                    vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite
                ),
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eFragmentShader,
                    vk::AccessFlagBits::eShaderRead
                )
            )
        );

        auto stageId = addRenderStage("omni_shadow");
        auto& pStage = getRenderStage(stageId);
        pStage->create(m_mStageInfos["omni_shadow"]);
    }

    {
        m_mStageInfos["deferred"].srName = "Deferred GBuffer pass";
        m_mStageInfos["deferred"].viewport.offset = vk::Offset2D(0, 0);
        m_mStageInfos["deferred"].viewport.extent = m_pDevice->getExtent(true);
        m_mStageInfos["deferred"].bFlipViewport = true;
        m_mStageInfos["deferred"].bViewportDependent = true;
        m_mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "albedo_tex", vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled, vk::SamplerAddressMode::eRepeat });
        m_mStageInfos["deferred"].vOutputs.emplace_back("albedo_tex");
        m_mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "normal_tex", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled, vk::SamplerAddressMode::eRepeat });
        m_mStageInfos["deferred"].vOutputs.emplace_back("normal_tex");
        m_mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "mrah_tex", vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled, vk::SamplerAddressMode::eRepeat });
        m_mStageInfos["deferred"].vOutputs.emplace_back("mrah_tex");
        m_mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "emission_tex", vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
        m_mStageInfos["deferred"].vOutputs.emplace_back("emission_tex");
        m_mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "depth_tex", depth_format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, vk::SamplerAddressMode::eRepeat });
        m_mStageInfos["deferred"].vDescriptions.emplace_back("depth_tex");
        
        if (CSessionStorage::getInstance()->get<bool>("editor_mode"))
        {
            m_mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "picking_tex", vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc });
            m_mStageInfos["deferred"].vOutputs.emplace_back("picking_tex");
        }
        
        m_mStageInfos["deferred"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eBottomOfPipe,
                    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite
                ),
                FCIDependencyDesc(
                    0,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
                    vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite
                )
            )
        );
        m_mStageInfos["deferred"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    0,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
                    vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite
                ),
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput,
                    vk::AccessFlagBits::eColorAttachmentWrite
                )
            )
        );

        auto stageId = addRenderStage("deferred");
        auto& pStage = getRenderStage(stageId);
        pStage->create(m_mStageInfos["deferred"]);
    }

    {
        m_mStageInfos["ssao"].srName = "Post Process SSAO pass";
        m_mStageInfos["ssao"].viewport.offset = vk::Offset2D(0, 0);
        m_mStageInfos["ssao"].viewport.extent = m_pDevice->getExtent(true);
        m_mStageInfos["ssao"].bFlipViewport = false;
        m_mStageInfos["ssao"].bViewportDependent = true;
        m_mStageInfos["ssao"].vImages.emplace_back(FCIImage{ "raw_ao_tex", vk::Format::eR8Unorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
        m_mStageInfos["ssao"].vDescriptions.emplace_back("");
        m_mStageInfos["ssao"].vOutputs.emplace_back("raw_ao_tex");

        m_mStageInfos["ssao"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput,
                    vk::AccessFlagBits::eColorAttachmentWrite
                ),
                FCIDependencyDesc(
                    0,
                    vk::PipelineStageFlagBits::eAllGraphics,
                    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite
                )
            )
        );
        m_mStageInfos["ssao"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    0,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput,
                    vk::AccessFlagBits::eColorAttachmentWrite
                ),
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput,
                    vk::AccessFlagBits::eColorAttachmentWrite
                )
            )
        );

        auto stageId = addRenderStage("ssao");
        auto& pStage = getRenderStage(stageId);
        pStage->create(m_mStageInfos["ssao"]);
    }

    {
        m_mStageInfos["composition"].srName = "Deferred composition pass";
        m_mStageInfos["composition"].viewport.offset = vk::Offset2D(0, 0);
        m_mStageInfos["composition"].viewport.extent = m_pDevice->getExtent(true);
        m_mStageInfos["composition"].bViewportDependent = true;
        m_mStageInfos["composition"].vImages.emplace_back(FCIImage{ "composition_tex", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
        m_mStageInfos["composition"].vImages.emplace_back(FCIImage{ "brightcolor_tex", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
        m_mStageInfos["composition"].vOutputs.emplace_back("composition_tex");
        m_mStageInfos["composition"].vOutputs.emplace_back("brightcolor_tex");
        m_mStageInfos["composition"].vDescriptions.emplace_back("");
        m_mStageInfos["composition"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput,
                    vk::AccessFlagBits::eColorAttachmentWrite
                ),
                FCIDependencyDesc(
                    0,
                    vk::PipelineStageFlagBits::eAllGraphics,
                    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite
                )
            )
        );
        m_mStageInfos["composition"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    0,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput,
                    vk::AccessFlagBits::eColorAttachmentWrite
                ),
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput,
                    vk::AccessFlagBits::eColorAttachmentWrite
                )
            )
        );

        auto stageId = addRenderStage("composition");
        auto& pStage = getRenderStage(stageId);
        pStage->create(m_mStageInfos["composition"]);
    }

    {
        m_mStageInfos["global_illumination"].srName = "Post Process Global Illumination pass";
        m_mStageInfos["global_illumination"].viewport.offset = vk::Offset2D(0, 0);
        m_mStageInfos["global_illumination"].viewport.extent = m_pDevice->getExtent(true);
        m_mStageInfos["global_illumination"].bFlipViewport = false;
        m_mStageInfos["global_illumination"].bViewportDependent = true;
        m_mStageInfos["global_illumination"].vImages.emplace_back(FCIImage{ "global_illumination_tex", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
        m_mStageInfos["global_illumination"].vDescriptions.emplace_back("");
        m_mStageInfos["global_illumination"].vOutputs.emplace_back("global_illumination_tex");

        m_mStageInfos["global_illumination"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput,
                    vk::AccessFlagBits::eColorAttachmentWrite
                ),
                FCIDependencyDesc(
                    0,
                    vk::PipelineStageFlagBits::eAllGraphics,
                    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite
                )
            )
        );
        m_mStageInfos["global_illumination"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    0,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput,
                    vk::AccessFlagBits::eColorAttachmentWrite
                ),
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput,
                    vk::AccessFlagBits::eColorAttachmentWrite
                )
            )
        );

        auto stageId = addRenderStage("global_illumination");
        auto& pStage = getRenderStage(stageId);
        pStage->create(m_mStageInfos["global_illumination"]);
    }

    {
        m_mStageInfos["ssr"].srName = "Post Process Screen Space Reflections pass";
        m_mStageInfos["ssr"].viewport.offset = vk::Offset2D(0, 0);
        m_mStageInfos["ssr"].viewport.extent = m_pDevice->getExtent(true);
        m_mStageInfos["ssr"].bFlipViewport = false;
        m_mStageInfos["ssr"].bViewportDependent = true;
        m_mStageInfos["ssr"].vImages.emplace_back(FCIImage{ "reflections_tex", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
        m_mStageInfos["ssr"].vDescriptions.emplace_back("");
        m_mStageInfos["ssr"].vOutputs.emplace_back("reflections_tex");

        m_mStageInfos["ssr"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput,
                    vk::AccessFlagBits::eColorAttachmentWrite
                ),
                FCIDependencyDesc(
                    0,
                    vk::PipelineStageFlagBits::eAllGraphics,
                    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite
                )
            )
        );
        m_mStageInfos["ssr"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    0,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput,
                    vk::AccessFlagBits::eColorAttachmentWrite
                ),
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput,
                    vk::AccessFlagBits::eColorAttachmentWrite
                )
            )
        );

        auto stageId = addRenderStage("ssr");
        auto& pStage = getRenderStage(stageId);
        pStage->create(m_mStageInfos["ssr"]);
    }
    
    {
        m_mStageInfos["present"].srName = "Present pass";
        m_mStageInfos["present"].viewport.offset = vk::Offset2D(0, 0);
        m_mStageInfos["present"].viewport.extent = m_pDevice->getExtent();
        m_mStageInfos["present"].vImages.emplace_back(FCIImage{ "present_khr", m_pDevice->getImageFormat(), vk::ImageUsageFlagBits::eColorAttachment });
        m_mStageInfos["present"].vOutputs.emplace_back("present_khr");
        m_mStageInfos["present"].vDescriptions.emplace_back("");
        m_mStageInfos["present"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eColorAttachmentOutput,
                    vk::AccessFlagBits::eColorAttachmentWrite
                ),
                FCIDependencyDesc(
                    0,
                    vk::PipelineStageFlagBits::eAllGraphics,
                    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite
                )
            )
        );

        auto stageId = addRenderStage("present");
        auto& pStage = getRenderStage(stageId);
        pStage->create(m_mStageInfos["present"]);
    }

    m_pQueryPool->create();

    log_info("Graphics core initialized.");
}

void CAPIHandle::update()
{
    static size_t count_delete{ 0 };
    count_delete++;

    if(m_pBindlessTextures)
        m_pBindlessTextures->update();

    if (count_delete > m_pDevice->getFramesInFlight() + 1)
    {
        m_pEvents->sendEvent(m_pFrameDoneEvent);
        forceReleaseResources();
        count_delete = 0;
    }
}

void CAPIHandle::forceReleaseResources()
{
    m_pImageManager->performDeletion();
    m_pShaderManager->performDeletion();
    m_pMaterialManager->performDeletion();
    m_pVertexBufferManager->performDeletion();
    m_pRenderStageManager->performDeletion();
}

void CAPIHandle::reCreate(bool bSwapchain, bool bViewport)
{
    // ReCreation guide
    // 
    // re create viewport only:
    // 1. gpu wait
    // 2. re create viewport framebuffer

    // re create swapchain
    // 1. gpu wait
    // 2. destroy swapchain (destroy all images)
    // 3. create swapchain (with images)
    // 4. re create framebuffers

    log_debug("ReCreating swapchain.");

    m_pDevice->GPUWait();

    while (m_pWindow->isMinimized())
        m_pWindow->processEvents();

    if (bSwapchain)
    {
        m_pDevice->recreateSwapchain(m_pWindow->getWidth(), m_pWindow->getHeight());
        m_imageIndex = 0;
    }

    for (auto& [name, stage] : m_mStageInfos)
    {
        if (stage.bIgnoreRecreation)
            continue;

        if (bViewport && !bSwapchain && !stage.bViewportDependent)
            continue;

        stage.viewport.extent = m_pDevice->getExtent(stage.bViewportDependent);
        auto& pStage = m_pRenderStageManager->get(name);
        pStage->reCreate(stage);
    }

    m_pEvents->sendEvent(m_pReCreateEvent);

    if (bViewport)
        m_pEvents->sendEvent(m_pViewportReCreateEvent);

    m_pDevice->nillViewportFlag();
}

void CAPIHandle::shutdown()
{
    m_pDevice->GPUWait();
    m_pDebugDraw = nullptr;
    m_pQueryPool = nullptr;
    m_pRenderStageManager = nullptr;
    m_pMaterialManager = nullptr;
    m_pVertexBufferManager = nullptr;
    m_pShaderManager = nullptr;
    m_pImageManager = nullptr;
}

void CAPIHandle::setVirtualFileSystem(filesystem::IVirtualFileSystemInterface* vfs_ptr)
{
    m_pVFS = vfs_ptr;
}

void CAPIHandle::setEventSystem(IEventManagerInterface* evt_ptr)
{
    m_pEvents = evt_ptr;
}

const std::unique_ptr<IDebugDrawInterface>& CAPIHandle::getDebugDraw() const
{
    return m_pDebugDraw;
}

const std::unique_ptr<CQueryPool>& CAPIHandle::getQueryPool() const
{
    return m_pQueryPool;
}

vk::CommandBuffer CAPIHandle::begin(bool& bRunning)
{
    bRunning = m_pWindow->processEvents();

    vk::CommandBuffer commandBuffer{};
    try { commandBuffer = beginFrame(); }
    catch (vk::OutOfDateKHRError err) { reCreate(true, true); }
    catch (vk::SystemError err) { log_error("Failed to acquire swap chain image!"); }

    //if(commandBuffer)
    //    m_pQueryPool->clear(commandBuffer);

    return commandBuffer;
}

void CAPIHandle::end(float fDT)
{
    vk::Result resultPresent;
    try { resultPresent = endFrame(); }
    catch (vk::OutOfDateKHRError err) { resultPresent = vk::Result::eErrorOutOfDateKHR; }
    catch (vk::SystemError err) { log_error("failed to present swap chain image!"); }

    if (resultPresent == vk::Result::eSuboptimalKHR || resultPresent == vk::Result::eErrorOutOfDateKHR || m_pWindow->wasResized())
        reCreate(true, true);

    //m_pQueryPool->takeResult();
    m_pDevice->updateCommandPools();
}

vk::CommandBuffer CAPIHandle::getCommandBuffer()
{
    return m_pCommandBuffers->getCommandBuffer();
}

const std::unique_ptr<IWindowAdapter>& CAPIHandle::getWindow() const
{
    return m_pWindow;
}

const std::unique_ptr<CDevice>& CAPIHandle::getDevice() const
{
    return m_pDevice;
}

const std::unique_ptr<CShaderLoader>& CAPIHandle::getShaderLoader()
{
    return m_pShaderLoader;
}

const std::unique_ptr<CImageLoader>& CAPIHandle::getImageLoader()
{
    return m_pImageLoader;
}

const std::unique_ptr<CBindlessDescriptor>& CAPIHandle::getBindlessDescriptor() const
{
    return m_pBindlessTextures;
}

size_t CAPIHandle::addImage(const std::string& name, std::unique_ptr<CImage>&& image)
{
    auto usageFlags = image->getUsage();
    auto imageLayout = image->getLayout();
    auto img_id = m_pImageManager->add(name, std::move(image));

    if(m_pBindlessTextures && 
        (usageFlags & vk::ImageUsageFlagBits::eSampled) && 
        !(usageFlags & vk::ImageUsageFlagBits::eColorAttachment) &&
        (usageFlags & vk::ImageUsageFlagBits::eTransferSrc) &&
        (usageFlags & vk::ImageUsageFlagBits::eTransferDst))
        m_pBindlessTextures->addTexture(img_id);

    return img_id;
}

size_t CAPIHandle::addImage(const std::string& name, const std::string& path, vk::Format overrideFormat)
{
    std::unique_ptr<CImage> image = std::make_unique<CImage>(m_pDevice.get());

    if (m_pImageManager->getId(name) == invalid_index)
    {
        if (fs::is_ktx_format(path))
            image->create(path);
        else
            image->create(path, overrideFormat);
    }

    return addImage(name, std::move(image));
}

size_t CAPIHandle::addImageAsync(const std::string& name, const std::string& path)
{
    std::unique_ptr<CImage> image = std::make_unique<CImage>(m_pDevice.get());

    if (m_pImageManager->getId(name) == invalid_index)
    {
        if (fs::is_ktx_format(path))
            image->create(path);
        else
            image->create(path, vk::Format::eR8G8B8A8Unorm);
    }

    return addImage(name, std::move(image));
}

void CAPIHandle::incrementImageUsage(const std::string& name)
{
    m_pImageManager->increment(name);
}

void CAPIHandle::incrementImageUsage(size_t id)
{
    m_pImageManager->increment(id);
}

void CAPIHandle::removeImage(const std::string& name)
{
    m_pImageManager->remove(name);
}

void CAPIHandle::removeImage(size_t id)
{
    m_pImageManager->remove(id);
}

const std::unique_ptr<CImage>& CAPIHandle::getImage(const std::string& name)
{
    return m_pImageManager->get(name);
}

size_t CAPIHandle::getImageID(const std::string& name)
{
    return m_pImageManager->getId(name);
}

const std::unique_ptr<CImage>& CAPIHandle::getImage(size_t id)
{
    return m_pImageManager->get(id);
}

void CAPIHandle::copyImage(vk::CommandBuffer& commandBuffer, size_t src, size_t dst)
{
    auto& src_image = getImage(src);
    auto& dst_image = getImage(dst);

    vk::ImageCopy region;
    region.extent = src_image->getExtent();
    // src
    region.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    region.srcSubresource.baseArrayLayer = 0;
    region.srcSubresource.layerCount = src_image->getLayers();
    region.srcSubresource.mipLevel = 0;
    // dst
    region.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    region.dstSubresource.baseArrayLayer = 0;
    region.dstSubresource.layerCount = dst_image->getLayers();
    region.dstSubresource.mipLevel = 0;

    auto& src_vk_image = src_image->getImage();
    auto& dst_vk_image = dst_image->getImage();
    auto src_layout = src_image->getLayout();
    auto dst_layout = dst_image->getLayout();

    CImage::copyTo(commandBuffer, src_vk_image, dst_vk_image, src_layout, dst_layout, region);
}


size_t CAPIHandle::addShader(const std::string& name, std::unique_ptr<CShaderObject>&& shader)
{
    return m_pShaderManager->add(name, std::move(shader));
}

size_t CAPIHandle::addShader(const std::string& shadertype, const FShaderCreateInfo& specials)
{
    std::vector<FShaderModuleCreateInfo> modules_ci{};
    auto shader_id_name = m_pShaderLoader->getShaderCreateInfo(shadertype, specials, modules_ci);

    size_t shader_id{ invalid_index };
    if (auto& shader = getShader(shader_id_name))
    {
        shader_id = getShaderID(shader_id_name);
        shader->increaseUsage(specials.usages);
        m_pShaderManager->increment(shader_id);
    }
    else
        shader_id = addShader(shader_id_name, m_pShaderLoader->load(modules_ci, specials));

    return shader_id;
}

void CAPIHandle::removeShader(const std::string& name)
{
    // TODO: Decrement usages
    m_pShaderManager->remove(name);
}

void CAPIHandle::removeShader(size_t id)
{
    m_pShaderManager->remove(id);
}

const std::unique_ptr<CShaderObject>& CAPIHandle::getShader(const std::string& name)
{
    return m_pShaderManager->get(name);
}

size_t CAPIHandle::getShaderID(const std::string& name)
{
    return m_pShaderManager->getId(name);
}

const std::unique_ptr<CShaderObject>& CAPIHandle::getShader(size_t id)
{
    return m_pShaderManager->get(id);
}


size_t CAPIHandle::addMaterial(const std::string& name, std::unique_ptr<CMaterial>&& material)
{
    return m_pMaterialManager->add(name, std::move(material));
}

void CAPIHandle::removeMaterial(const std::string& name)
{
    auto& material = getMaterial(name);
    if (material)
    {
        removeShader(material->getShader());
        for (auto& [name, tex_id] : material->getTextures())
            removeImage(tex_id);
    }

    m_pMaterialManager->remove(name);
}

void CAPIHandle::removeMaterial(size_t id)
{
    auto& material = getMaterial(id);
    if (material)
    {
        removeShader(material->getShader());
        for (auto& [name, tex_id] : material->getTextures())
            removeImage(tex_id);
    }

    m_pMaterialManager->remove(id);
}

const std::unique_ptr<CMaterial>& CAPIHandle::getMaterial(const std::string& name)
{
    return m_pMaterialManager->get(name);
}

const std::unique_ptr<CMaterial>& CAPIHandle::getMaterial(size_t id)
{
    return m_pMaterialManager->get(id);
}



size_t CAPIHandle::addVertexBuffer(const std::string& name)
{
    return m_pVertexBufferManager->add(name, std::make_unique<CVertexBufferObject>(m_pDevice.get()));
}

size_t CAPIHandle::addVertexBuffer(const std::string& name, std::unique_ptr<CVertexBufferObject>&& vbo)
{
    return m_pVertexBufferManager->add(name, std::move(vbo));
}

void CAPIHandle::removeVertexBuffer(const std::string& name)
{
    m_pVertexBufferManager->remove(name);
}

void CAPIHandle::removeVertexBuffer(size_t id)
{
    m_pVertexBufferManager->remove(id);
}

const std::unique_ptr<CVertexBufferObject>& CAPIHandle::getVertexBuffer(const std::string& name)
{
    return m_pVertexBufferManager->get(name);
}

const std::unique_ptr<CVertexBufferObject>& CAPIHandle::getVertexBuffer(size_t id)
{
    return m_pVertexBufferManager->get(id);
}


size_t CAPIHandle::addRenderStage(const std::string& name)
{
    return m_pRenderStageManager->add(name, std::make_unique<CRenderStage>(m_pDevice.get()));
}

size_t CAPIHandle::addRenderStage(const std::string& name, std::unique_ptr<CRenderStage>&& stage)
{
    return m_pRenderStageManager->add(name, std::move(stage));
}

void CAPIHandle::removeRenderStage(const std::string& name)
{
    m_pRenderStageManager->remove(name);
}

void CAPIHandle::removeRenderStage(size_t id)
{
    m_pRenderStageManager->remove(id);
}

const std::unique_ptr<CRenderStage>& CAPIHandle::getRenderStage(const std::string& name)
{
    return m_pRenderStageManager->get(name);
}

const std::unique_ptr<CRenderStage>& CAPIHandle::getRenderStage(size_t id)
{
    return m_pRenderStageManager->get(id);
}

size_t CAPIHandle::getRenderStageID(const std::string& name)
{
    return m_pRenderStageManager->getId(name);
}

const size_t CAPIHandle::getDrawCallCount() const
{
    return drawCallCount;
}

const std::unique_ptr<CFramebuffer>& CAPIHandle::getFramebuffer(const std::string& srName)
{
    return getRenderStage(srName)->getFramebuffer();
}

size_t CAPIHandle::computeBRDFLUT(uint32_t size)
{
    FDispatchParam param;
    param.size = { size, size, 1u };

    auto brdfImage = std::make_unique<CImage2D>(m_pDevice.get());
    brdfImage->create(
        vk::Extent2D{ size, size },
        vk::Format::eR16G16Sfloat,
        vk::ImageLayout::eGeneral,
        vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment |
        vk::ImageUsageFlagBits::eStorage);

    auto output_id = addImage("brdflut", std::move(brdfImage));

    FShaderCreateInfo shader_ci{};
    shader_ci.bind_point = vk::PipelineBindPoint::eCompute;
    shader_ci.usages = 1;

    auto shader_id = addShader("generators:brdflut_generator", shader_ci);
    auto& pShader = getShader(shader_id);

    pShader->addTexture("outColour", output_id);

    pShader->dispatch(param);

    removeShader(shader_id);

    return output_id;
}

size_t CAPIHandle::computeIrradiance(size_t origin, uint32_t size)
{
    FDispatchParam param;
    param.size = { size, size, 1u };

    std::vector<glm::vec3> sizes{ { size, size, 1u } };
    auto irradianceCubemap = std::make_unique<CImageCubemap>(m_pDevice.get());
    irradianceCubemap->create(
        vk::Extent2D{ size, size },
        vk::Format::eR32G32B32A32Sfloat,
        vk::ImageLayout::eGeneral,
        vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment |
        vk::ImageUsageFlagBits::eStorage);

    auto output_id = addImage("irradiance", std::move(irradianceCubemap));

    FShaderCreateInfo shader_ci{};
    shader_ci.bind_point = vk::PipelineBindPoint::eCompute;
    shader_ci.usages = 1;

    auto shader_id = addShader("generators:irradiancecube_generator", shader_ci);
    auto& pShader = getShader(shader_id);
    pShader->addTexture("outColour", output_id);
    pShader->addTexture("samplerColor", origin);

    pShader->dispatch(param);

    removeShader(shader_id);

    return output_id;
}

size_t CAPIHandle::computePrefiltered(size_t origin, uint32_t size)
{
    FDispatchParam param;
    param.size = { size, size, 1u };

    std::vector<glm::vec3> sizes{ { size, size, 1u } };
    auto prefilteredCubemap = std::make_unique<CImageCubemap>(m_pDevice.get());
    prefilteredCubemap->create(
        vk::Extent2D{ size, size },
        vk::Format::eR16G16B16A16Sfloat,
        vk::ImageLayout::eGeneral,
        vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment |
        vk::ImageUsageFlagBits::eStorage,
        vk::ImageAspectFlagBits::eColor,
        vk::Filter::eLinear,
        vk::SamplerAddressMode::eClampToEdge,
        vk::SampleCountFlagBits::e1, true, false, true);

    auto output_id = addImage("prefiltered", std::move(prefilteredCubemap));
    auto& target = getImage(output_id);

    FShaderCreateInfo shader_ci{};
    shader_ci.bind_point = vk::PipelineBindPoint::eCompute;
    shader_ci.usages = 1;

    auto shader_id = addShader("generators:prefilteredmap_generator", shader_ci);
    auto& pShader = getShader(shader_id);
    auto& pPushConst = pShader->getPushBlock("object");

    auto cmdBuf = CCommandBuffer(m_pDevice.get());
    cmdBuf.create(false, vk::QueueFlagBits::eCompute);

    for (uint32_t i = 0; i < target->getMipLevels(); i++)
    {
        vk::ImageView levelView = VK_NULL_HANDLE;
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.viewType = vk::ImageViewType::eCube;
        viewInfo.format = target->getFormat();
        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.baseMipLevel = i;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 6;
        viewInfo.image = target->getImage();

        vk::Result res = m_pDevice->create(viewInfo, &levelView);
        assert(res == vk::Result::eSuccess && "Cannot create image view.");

        cmdBuf.begin();
        auto commandBuffer = cmdBuf.getCommandBuffer();

        auto imageInfo = target->getDescriptor();
        imageInfo.imageView = levelView;

        pPushConst->set("roughness", static_cast<float>(i) / static_cast<float>(target->getMipLevels() - 1));
        pPushConst->flush(commandBuffer);

        pShader->addTexture("outColour", imageInfo);
        pShader->addTexture("samplerColor", origin);

        pShader->dispatch(commandBuffer, param);
        cmdBuf.submitIdle();

        m_pDevice->destroy(&levelView);
    }

    removeShader(shader_id);

    return output_id;
}

size_t CAPIHandle::compute2DNoise(const std::string& imageName, vk::Extent2D noiseDim)
{
    FDispatchParam param;
    param.size = { noiseDim.width, noiseDim.height, 1 };

    auto noiseTexture = std::make_unique<CImage2D>(m_pDevice.get());
    noiseTexture->create(
        noiseDim,
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageLayout::eGeneral,
        vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment |
        vk::ImageUsageFlagBits::eStorage,
        vk::ImageAspectFlagBits::eColor,
        vk::Filter::eLinear,
        vk::SamplerAddressMode::eClampToEdge,
        vk::SampleCountFlagBits::e1);

    auto output_id = addImage(imageName, std::move(noiseTexture));

    FShaderCreateInfo shader_ci{};
    shader_ci.bind_point = vk::PipelineBindPoint::eCompute;
    shader_ci.usages = 1;

    auto shader_id = addShader("noise:2dnoisegen", shader_ci);
    auto& pShader = getShader(shader_id);
    pShader->addTexture("outNoise", output_id);

    pShader->dispatch(param);

    removeShader(shader_id);

    return output_id;
}

size_t CAPIHandle::compute3DNoise(const std::string& imageName, const std::string& shaderName, vk::Extent3D noiseDim)
{
    FDispatchParam param;
    param.size = { noiseDim.width, noiseDim.height, noiseDim.depth };

    auto noiseTexture = std::make_unique<CImage3D>(m_pDevice.get());
    noiseTexture->create(
        noiseDim,
        vk::Format::eR8Unorm,
        vk::ImageLayout::eGeneral,
        vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment |
        vk::ImageUsageFlagBits::eStorage,
        vk::ImageAspectFlagBits::eColor,
        vk::Filter::eLinear,
        vk::SamplerAddressMode::eClampToEdge,
        vk::SampleCountFlagBits::e1);

    auto output_id = addImage(imageName, std::move(noiseTexture));

    FShaderCreateInfo shader_ci{};
    shader_ci.bind_point = vk::PipelineBindPoint::eGraphics;
    shader_ci.usages = 1;

    auto shader_id = addShader(shaderName, shader_ci);
    auto& pShader = getShader(shader_id);
    pShader->addTexture("outNoise", output_id);

    pShader->dispatch(param);

    removeShader(shader_id);

    return output_id;
}

// Draw state machine
void CAPIHandle::bindShader(size_t id)
{
    static size_t timesBind{ 0ull };
    if (id == invalid_index)
    {
        m_pBindedShader = nullptr;
        return;
    }

    auto& shader = getShader(id);
    if (shader && shader.get() != m_pBindedShader)
    {
        auto& commandBuffer = m_pCommandBuffers->getCommandBuffer();
        m_pBindedShader = shader.get();
        m_pBindedShader->bind(commandBuffer);

        if (m_pBindedShader->isUsesBindlessTextures())
        {
            auto& pipeline = m_pBindedShader->getPipeline();
            m_pBindlessTextures->bind(commandBuffer, pipeline->getBindPoint(), pipeline->getPipelineLayout());
        }

        timesBind = 1;
    }        
    else
        timesBind++;
}

bool CAPIHandle::bindMaterial(size_t id)
{
    static size_t timesBind{ 0ull };
    if (id == invalid_index)
    {
        m_pBindedMaterial = nullptr;
        m_pBindedShader = nullptr;
        return false;
    }

    auto& material = getMaterial(id);
    if (material && material.get() != m_pBindedMaterial)
    {
        m_pBindedMaterial = material.get();
        bindShader(m_pBindedMaterial->getShader());

        auto& pSurface = getUniformHandle("UBOMaterial");
        if (pSurface)
        {
            auto& params = m_pBindedMaterial->getParameters();

            pSurface->set("baseColorFactor", params.baseColorFactor);
            pSurface->set("emissiveFactor", params.emissiveFactor);
            pSurface->set("emissiveStrength", params.emissiveStrength);
            pSurface->set("alphaMode", static_cast<int>(params.alphaMode));
            pSurface->set("alphaCutoff", params.alphaCutoff);
            pSurface->set("normalScale", params.normalScale);
            pSurface->set("occlusionStrenth", params.occlusionStrenth);
            pSurface->set("metallicFactor", params.metallicFactor);
            pSurface->set("roughnessFactor", params.roughnessFactor);
            pSurface->set("tessellationFactor", params.tessellationFactor);
            pSurface->set("displacementStrength", params.displacementStrength);
        }

        auto& pUBO = getUniformHandle("FUniformData");
        if (pUBO)
        {
            auto invView = glm::inverse(*m_ViewMatrix);
            pUBO->set("view", *m_ViewMatrix);
            pUBO->set("projection", *m_ProjectionMatrix);
            pUBO->set("viewDir", glm::vec3(invView[3]));
            pUBO->set("viewportDim", m_pDevice->getExtent(true));
            pUBO->set("frustumPlanes", *m_pFrustumPlanes);
        }

        auto& textures = m_pBindedMaterial->getTextures();
        for (auto& [name, id] : textures)
            bindTexture(name, id);

        //auto& commandBuffer = m_pCommandBuffers->getCommandBuffer();
        //m_pBindedShader->predraw(commandBuffer);

        timesBind = 1;

        return true;
    }

    timesBind++;

    return false;
}

bool CAPIHandle::bindVertexBuffer(size_t id)
{
    if (id == invalid_index)
    {
        m_pBindedVBO = nullptr;
        return false;
    }
    
    auto& vbo = getVertexBuffer(id);
    if (vbo && vbo.get() != m_pBindedVBO && vbo->isLoaded())
    {
        auto& commandBuffer = m_pCommandBuffers->getCommandBuffer();

        m_pBindedVBO = vbo.get();
        m_pBindedVBO->bind(commandBuffer);
    }

    return vbo->isLoaded();
}

void CAPIHandle::bindRenderer(size_t id)
{
    if (id == invalid_index)
    {
        if (m_pBindedRenderStage)
        {
            auto& commandBuffer = m_pCommandBuffers->getCommandBuffer();

            m_pBindedRenderStage->end(commandBuffer);
            m_pBindedRenderStage = nullptr;
        }
        // Log here?

        return;
    }

    auto& renderStage = getRenderStage(id);
    if (renderStage)
    {
        auto& commandBuffer = m_pCommandBuffers->getCommandBuffer();

        m_pBindedRenderStage = renderStage.get();
        m_pBindedRenderStage->begin(commandBuffer);
    }
    else
        log_error("Cannot bind render stage, because stage with id {} is not exists!");
}

void CAPIHandle::bindTexture(const std::string& name, size_t id, uint32_t mip_level)
{
    if(!m_pBindedShader)
        log_error("Cannot bind texture, cause shader was not binded.");

    if (m_pBindlessTextures && m_pBindedShader->isUsesBindlessTextures())
    {
        auto& pBindlessTextures = getUniformHandle("UBOMaterialTextures");
        if (pBindlessTextures)
            pBindlessTextures->set(name, id);
    }
    else
        m_pBindedShader->addTexture(name, id, mip_level);
}

void CAPIHandle::bindObjectData(const glm::mat4& model, const glm::mat4& normal, uint32_t object_id)
{
    auto& pMesh = getUniformHandle("UBOMeshData");
    if (pMesh)
    {
        pMesh->set("model", model);
        pMesh->set("normal", normal);
        pMesh->set("object_id", encodeIdToColor(object_id));
    }
}

void CAPIHandle::bindCameraData(const glm::mat4& view, const glm::mat4& projection, const std::array<std::array<float, 4>, 6>& frustumPlanes)
{
    m_ViewMatrix = &view;
    m_ProjectionMatrix = &projection;
    m_pFrustumPlanes = &frustumPlanes;
}

void CAPIHandle::clearQuery()
{
    auto& commandBuffer = m_pCommandBuffers->getCommandBuffer();
    m_pQueryPool->clear(commandBuffer);
}

void CAPIHandle::beginQuery(uint32_t index)
{
    auto& commandBuffer = m_pCommandBuffers->getCommandBuffer();
    m_pQueryPool->begin(commandBuffer, index);
}

void CAPIHandle::endQuery(uint32_t index)
{
    auto& commandBuffer = m_pCommandBuffers->getCommandBuffer();
    m_pQueryPool->end(commandBuffer, index);
}

void CAPIHandle::getQueryResult()
{
    m_pQueryPool->takeResult();
}

bool CAPIHandle::occlusionTest(uint32_t index)
{
    return m_pQueryPool->wasDrawn(index);
}

bool CAPIHandle::compareAlphaMode(EAlphaMode mode)
{
    if (m_pBindedMaterial)
    {
        auto& params = m_pBindedMaterial->getParameters();
        return params.alphaMode == mode;
    }

    log_error("Cannot compare alpha mode, cause material was not binded.");

    return false;
}

bool CAPIHandle::compareAlphaMode(size_t material_id, EAlphaMode mode)
{
    auto& material = getMaterial(material_id);
    if (material)
    {
        auto& params = material->getParameters();
        return params.alphaMode == mode;
    }

    log_error("Cannot compare alpha mode, cause material was not found.");

    return false;
}

void CAPIHandle::flushConstantRanges(const std::unique_ptr<CPushHandler>& constantRange)
{
    auto& commandBuffer = m_pCommandBuffers->getCommandBuffer();
    constantRange->flush(commandBuffer);
}

void CAPIHandle::flushShader()
{
    auto& commandBuffer = m_pCommandBuffers->getCommandBuffer();

    if (m_pBindedShader)
        m_pBindedShader->predraw(commandBuffer);
    else
        log_error("Cannot flush shader data, cause shader is not binded.");
}

const std::unique_ptr<CHandler>& CAPIHandle::getUniformHandle(const std::string& name)
{
    static std::unique_ptr<CHandler> pEmpty{ nullptr };
    if (m_pBindedShader)
        return m_pBindedShader->getUniformBuffer(name);

    return pEmpty;
}

const std::unique_ptr<CPushHandler>& CAPIHandle::getPushBlockHandle(const std::string& name)
{
    static std::unique_ptr<CPushHandler> pEmpty{ nullptr };
    if (m_pBindedShader)
        return m_pBindedShader->getPushBlock(name);

    return pEmpty;
}

void CAPIHandle::draw(size_t begin_vertex, size_t vertex_count, size_t begin_index, size_t index_count, size_t instance_count)
{
    auto& commandBuffer = m_pCommandBuffers->getCommandBuffer();

    if (m_pBindedVBO)
    {
        auto last_index = m_pBindedVBO->getLastIndex();
        auto last_vertex = m_pBindedVBO->getLastIndex();

        if (last_index == 0)
        {
            // Vertex only pass
            vertex_count = vertex_count == 0 ? last_vertex : vertex_count;
            commandBuffer.draw(vertex_count, instance_count, begin_vertex, 0);
        }
        else
        {
            // Indexed drawing
            index_count = index_count == 0 ? last_index : index_count;
            commandBuffer.drawIndexed(index_count, instance_count, begin_index, 0, 0);
        }
    }
    else
        // Screen space drawing
        commandBuffer.draw(vertex_count, instance_count, begin_vertex, 0);

    ++drawCallCount;
}

void CAPIHandle::dispatch(const std::vector<FDispatchParam>& params)
{
    auto& commandBuffer = m_pCommandBuffers->getCommandBuffer();

    if (m_pBindedShader)
        m_pBindedShader->dispatch(commandBuffer, params);
    else
        log_error("Cannot dispatch, cause shader was not binded.")
}

void CAPIHandle::dispatch(const FDispatchParam& param)
{
    std::vector<FDispatchParam> params{ param };
    dispatch(params);
}


void CAPIHandle::BarrierFromComputeToCompute()
{
    auto commandBuffer = getCommandBuffer();
    BarrierFromComputeToCompute(commandBuffer);
}

void CAPIHandle::BarrierFromComputeToCompute(vk::CommandBuffer& commandBuffer)
{
    vk::MemoryBarrier2KHR barrier{};
    barrier.srcStageMask = vk::PipelineStageFlagBits2::eComputeShader;
    barrier.srcAccessMask = vk::AccessFlagBits2::eShaderWrite;
    barrier.dstStageMask = vk::PipelineStageFlagBits2::eComputeShader;
    barrier.dstAccessMask = vk::AccessFlagBits2::eShaderRead;

    APICompatibility::memoryBarrierCompat(commandBuffer, barrier);
}

void CAPIHandle::BarrierFromComputeToGraphics()
{
    auto commandBuffer = getCommandBuffer();
    BarrierFromComputeToGraphics(commandBuffer);
}

void CAPIHandle::BarrierFromComputeToGraphics(vk::CommandBuffer& commandBuffer)
{
    vk::MemoryBarrier2KHR barrier{};
    barrier.srcStageMask = vk::PipelineStageFlagBits2::eComputeShader;
    barrier.srcAccessMask = vk::AccessFlagBits2::eShaderWrite;
    barrier.dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader;
    barrier.dstAccessMask = vk::AccessFlagBits2::eShaderRead;

    APICompatibility::memoryBarrierCompat(commandBuffer, barrier);
}

void CAPIHandle::BarrierFromGraphicsToCompute(size_t image_id)
{
    auto commandBuffer = getCommandBuffer();
    BarrierFromGraphicsToCompute(commandBuffer, image_id);
}

void CAPIHandle::BarrierFromGraphicsToCompute(vk::CommandBuffer& commandBuffer, size_t image_id)
{
    auto& image = getImage(image_id);
    if (image)
    {
        vk::ImageMemoryBarrier2KHR imageMemoryBarrier{};
        imageMemoryBarrier.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
        imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
        imageMemoryBarrier.dstStageMask = vk::PipelineStageFlagBits2::eComputeShader;
        imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits2::eShaderRead;
        imageMemoryBarrier.oldLayout = vk::ImageLayout::eUndefined;
        imageMemoryBarrier.newLayout = image->getLayout();
        imageMemoryBarrier.image = image->getImage();
        imageMemoryBarrier.subresourceRange.aspectMask = image->getAspectMask();
        imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
        imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
        imageMemoryBarrier.subresourceRange.layerCount = image->getLayers();
        imageMemoryBarrier.subresourceRange.levelCount = image->getMipLevels();

        APICompatibility::imageMemoryBarrierCompat(commandBuffer, imageMemoryBarrier);
    }
}

void CAPIHandle::BarrierFromGraphicsToTransfer(size_t image_id)
{
    auto commandBuffer = getCommandBuffer();
    BarrierFromGraphicsToTransfer(commandBuffer, image_id);
}

void CAPIHandle::BarrierFromGraphicsToTransfer(vk::CommandBuffer& commandBuffer, size_t image_id)
{
    auto& image = getImage(image_id);
    if (image)
    {
        vk::ImageMemoryBarrier2KHR imageMemoryBarrier{};
        imageMemoryBarrier.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
        imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
        imageMemoryBarrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
        imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits2::eTransferRead | vk::AccessFlagBits2::eTransferWrite;
        imageMemoryBarrier.oldLayout = vk::ImageLayout::eUndefined;
        imageMemoryBarrier.newLayout = image->getLayout();
        imageMemoryBarrier.image = image->getImage();
        imageMemoryBarrier.subresourceRange.aspectMask = image->getAspectMask();
        imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
        imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
        imageMemoryBarrier.subresourceRange.layerCount = image->getLayers();
        imageMemoryBarrier.subresourceRange.levelCount = image->getMipLevels();

        APICompatibility::imageMemoryBarrierCompat(commandBuffer, imageMemoryBarrier);
    }
}



vk::CommandBuffer CAPIHandle::beginFrame()
{
    bool bSwapchain{ false }, bViewport{ m_pDevice->isViewportResized() };

    // Here we able to recreate viewport
    if (bViewport)
    {
        reCreate(bSwapchain, bViewport);
        return nullptr;
    }

    log_cerror(!m_bFrameStarted, "Can't call beginFrame while already in progress");
    vk::Result res = m_pDevice->acquireNextImage(&m_imageIndex);
    if (res == vk::Result::eErrorOutOfDateKHR)
        bSwapchain = true;
    else if (res == vk::Result::eSuboptimalKHR)
        log_error("Failed to acquire swap chain image!");

    // But here we able to recreate swapchain
    if (bSwapchain)
    {
        reCreate(bSwapchain, true);
        return nullptr;
    }

    m_bFrameStarted = true;

    m_pCommandBuffers->begin(vk::CommandBufferUsageFlagBits::eRenderPassContinue, m_imageIndex);
    return m_pCommandBuffers->getCommandBuffer();
}

vk::Result CAPIHandle::endFrame()
{
    log_cerror(m_bFrameStarted, "Can't call endFrame while frame is not in progress");
    m_pCommandBuffers->end();
    m_bFrameStarted = false;
    drawCallCount = 0ull;
    return m_pCommandBuffers->submit(m_imageIndex);
}