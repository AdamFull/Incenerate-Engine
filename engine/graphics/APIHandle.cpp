#include "APIHandle.h"

#include "system/window/WindowHandle.h"
#include "system/filesystem/filesystem.h"
#include "Engine.h"

#include "image/Image2D.h"
#include "image/ImageCubemap.h"

#include <glm/gtc/type_ptr.hpp>

using namespace engine::graphics;
using namespace engine::system;
using namespace engine::ecs;
using namespace engine::system::window;

CAPIHandle::CAPIHandle(winhandle_t window)
{
    pWindow = window;
}

CAPIHandle::~CAPIHandle()
{
    
}

void CAPIHandle::create(const FEngineCreateInfo& createInfo)
{
	eAPI = createInfo.eAPI;

    pImageManager = std::make_unique<CObjectManager<CImage>>();
    pShaderManager = std::make_unique<CObjectManager<CShaderObject>>();
    pMaterialManager = std::make_unique<CObjectManager<CMaterial>>();
    pVertexBufferManager = std::make_unique<CObjectManager<CVertexBufferObject>>();
    pRenderStageManager = std::make_unique<CObjectManager<CRenderStage>>();

	pDevice = std::make_unique<CDevice>(this);
	pDevice->create(createInfo);

    pLoader = std::make_unique<CShaderLoader>(pDevice.get());
    pLoader->create();
    
    commandBuffers = std::make_unique<CCommandBuffer>(pDevice.get());
    commandBuffers->create(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, pDevice->getFramesInFlight());

    pDebugDraw = std::make_unique<CDebugDraw>(this);
    pQueryPool = std::make_unique<CQueryPool>(pDevice.get());

    auto empty_image_2d = std::make_unique<CImage2D>(pDevice.get());
    empty_image_2d->create(vk::Extent2D{ 1u, 1u }, vk::Format::eR8G8B8A8Srgb);
    addImage("empty_image_2d", std::move(empty_image_2d));

    auto depth_format = pDevice->getDepthFormat();
    
    {
        mStageInfos["cascade_shadow"].srName = "cascade_shadow";
        mStageInfos["cascade_shadow"].viewport.offset = vk::Offset2D(0, 0);
        mStageInfos["cascade_shadow"].viewport.extent = vk::Extent2D(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
        mStageInfos["cascade_shadow"].bIgnoreRecreation = true;
        mStageInfos["cascade_shadow"].bFlipViewport = false;
        mStageInfos["cascade_shadow"].vImages.emplace_back("cascade_shadowmap_tex", depth_format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, EImageType::eArray2D, SHADOW_MAP_CASCADE_COUNT);
        mStageInfos["cascade_shadow"].vDescriptions.emplace_back("cascade_shadowmap_tex");
        mStageInfos["cascade_shadow"].vDependencies.emplace_back(
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
        mStageInfos["cascade_shadow"].vDependencies.emplace_back(
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
        pStage->create(mStageInfos["cascade_shadow"]);
    }

    {
        mStageInfos["direct_shadow"].srName = "direct_shadow";
        mStageInfos["direct_shadow"].viewport.offset = vk::Offset2D(0, 0);
        mStageInfos["direct_shadow"].viewport.extent = vk::Extent2D(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
        mStageInfos["direct_shadow"].bIgnoreRecreation = true;
        mStageInfos["direct_shadow"].bFlipViewport = false;
        mStageInfos["direct_shadow"].vImages.emplace_back("direct_shadowmap_tex", depth_format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, EImageType::eArray2D, MAX_SPOT_LIGHT_COUNT);
        mStageInfos["direct_shadow"].vDescriptions.emplace_back("direct_shadowmap_tex");
        mStageInfos["direct_shadow"].vDependencies.emplace_back(
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
        mStageInfos["direct_shadow"].vDependencies.emplace_back(
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
        pStage->create(mStageInfos["direct_shadow"]);
    }

    {
        mStageInfos["omni_shadow"].srName = "omni_shadow";
        mStageInfos["omni_shadow"].viewport.offset = vk::Offset2D(0, 0);
        mStageInfos["omni_shadow"].viewport.extent = vk::Extent2D(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
        mStageInfos["omni_shadow"].bIgnoreRecreation = true;
        mStageInfos["omni_shadow"].bFlipViewport = false;
        mStageInfos["omni_shadow"].vImages.emplace_back("omni_shadowmap_tex", depth_format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, EImageType::eArrayCube, MAX_POINT_LIGHT_COUNT);
        mStageInfos["omni_shadow"].vDescriptions.emplace_back("omni_shadowmap_tex");
        mStageInfos["omni_shadow"].vDependencies.emplace_back(
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
        mStageInfos["omni_shadow"].vDependencies.emplace_back(
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
        pStage->create(mStageInfos["omni_shadow"]);
    }


    {
        mStageInfos["deferred"].srName = "deferred";
        mStageInfos["deferred"].viewport.offset = vk::Offset2D(0, 0);
        mStageInfos["deferred"].viewport.extent = pDevice->getExtent(true);
        mStageInfos["deferred"].bFlipViewport = true;
        mStageInfos["deferred"].bViewportDependent = true;
        mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "albedo_tex", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
        mStageInfos["deferred"].vOutputs.emplace_back("albedo_tex");
        mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "normal_tex", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
        mStageInfos["deferred"].vOutputs.emplace_back("normal_tex");
        mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "mrah_tex", vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
        mStageInfos["deferred"].vOutputs.emplace_back("mrah_tex");
        mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "emission_tex", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
        mStageInfos["deferred"].vOutputs.emplace_back("emission_tex");
        mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "depth_tex", depth_format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled });
        mStageInfos["deferred"].vDescriptions.emplace_back("depth_tex");
        
        if (EGEngine->isEditorMode())
        {
            mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "picking_tex", vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc });
            mStageInfos["deferred"].vOutputs.emplace_back("picking_tex");
        }
        
        mStageInfos["deferred"].vDependencies.emplace_back(
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
        mStageInfos["deferred"].vDependencies.emplace_back(
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
        pStage->create(mStageInfos["deferred"]);
    }

    {
        mStageInfos["composition"].srName = "composition";
        mStageInfos["composition"].viewport.offset = vk::Offset2D(0, 0);
        mStageInfos["composition"].viewport.extent = pDevice->getExtent(true);
        mStageInfos["composition"].bViewportDependent = true;
        mStageInfos["composition"].vImages.emplace_back(FCIImage{ "composition_tex", vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled });
        mStageInfos["composition"].vOutputs.emplace_back("composition_tex");
        mStageInfos["composition"].vDescriptions.emplace_back("");
        mStageInfos["composition"].vDependencies.emplace_back(
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
        mStageInfos["composition"].vDependencies.emplace_back(
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
        pStage->create(mStageInfos["composition"]);
    }
    
    {
        mStageInfos["present"].srName = "present";
        mStageInfos["present"].viewport.offset = vk::Offset2D(0, 0);
        mStageInfos["present"].viewport.extent = pDevice->getExtent();
        mStageInfos["present"].vImages.emplace_back(FCIImage{ "present_khr", pDevice->getImageFormat(), vk::ImageUsageFlagBits::eColorAttachment });
        mStageInfos["present"].vOutputs.emplace_back("present_khr");
        mStageInfos["present"].vDescriptions.emplace_back("");
        mStageInfos["present"].vDependencies.emplace_back(
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
        pStage->create(mStageInfos["present"]);
    }

    pDebugDraw->create();
    pQueryPool->create();

    log_info("Graphics core initialized.");
}

void CAPIHandle::update()
{
    static size_t count_delete{ 0 };
    count_delete++;

    if (count_delete > pDevice->getFramesInFlight() + 1)
    {
        EGEngine->sendEvent(Events::Graphics::AllFramesDone);
        forceReleaseResources();
        count_delete = 0;
    }
}

void CAPIHandle::forceReleaseResources()
{
    pImageManager->performDeletion();
    pShaderManager->performDeletion();
    pMaterialManager->performDeletion();
    pVertexBufferManager->performDeletion();
    pRenderStageManager->performDeletion();
}

void CAPIHandle::reCreate(bool bSwapchain, bool bViewport)
{
    //ReCreation guide
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

    pDevice->GPUWait();

    while (pWindow->isMinimized())
        pWindow->begin();

    if (bSwapchain)
    {
        pDevice->recreateSwapchain();
        imageIndex = 0;
        CWindowHandle::bWasResized = false;
    }

    for (auto& [name, stage] : mStageInfos)
    {
        if (stage.bIgnoreRecreation)
            continue;

        if (bViewport && !bSwapchain && !stage.bViewportDependent)
            continue;

        stage.viewport.extent = pDevice->getExtent(stage.bViewportDependent);
        auto& pStage = pRenderStageManager->get(name);
        pStage->reCreate(stage);
    }

    EGEngine->sendEvent(Events::Graphics::ReCreate);

    if (bViewport)
        EGEngine->sendEvent(Events::Graphics::ViewportReCreate);

    pDevice->nillViewportFlag();
}

void CAPIHandle::shutdown()
{
    pDevice->GPUWait();
    pDebugDraw = nullptr;
    pQueryPool = nullptr;
    pRenderStageManager = nullptr;
    pMaterialManager = nullptr;
    pVertexBufferManager = nullptr;
    pShaderManager = nullptr;
    pImageManager = nullptr;
}

const std::unique_ptr<CDebugDraw>& CAPIHandle::getDebugDraw() const
{
    return pDebugDraw;
}

const std::unique_ptr<CQueryPool>& CAPIHandle::getQueryPool() const
{
    return pQueryPool;
}

vk::CommandBuffer CAPIHandle::begin()
{
    vk::CommandBuffer commandBuffer{};
    try { commandBuffer = beginFrame(); }
    catch (vk::OutOfDateKHRError err) { reCreate(true, true); }
    catch (vk::SystemError err) { log_error("Failed to acquire swap chain image!"); }

    //if(commandBuffer)
    //    pQueryPool->clear(commandBuffer);

    return commandBuffer;
}

void CAPIHandle::end(float fDT)
{
    vk::Result resultPresent;
    try { resultPresent = endFrame(); }
    catch (vk::OutOfDateKHRError err) { resultPresent = vk::Result::eErrorOutOfDateKHR; }
    catch (vk::SystemError err) { log_error("failed to present swap chain image!"); }

    if (resultPresent == vk::Result::eSuboptimalKHR || resultPresent == vk::Result::eErrorOutOfDateKHR || CWindowHandle::bWasResized)
        reCreate(true, true);

    //pQueryPool->takeResult();
    pDevice->updateCommandPools();
}

vk::CommandBuffer CAPIHandle::getCommandBuffer()
{
    return commandBuffers->getCommandBuffer();
}

const std::unique_ptr<CDevice>& CAPIHandle::getDevice() const
{
    return pDevice;
}

const std::unique_ptr<CShaderLoader>& CAPIHandle::getShaderLoader()
{
    return pLoader;
}

size_t CAPIHandle::addImage(const std::string& name, std::unique_ptr<CImage>&& image)
{
    return pImageManager->add(name, std::move(image));
}

size_t CAPIHandle::addImage(const std::string& name, const std::filesystem::path& path)
{
    std::unique_ptr<CImage> image = std::make_unique<CImage>(pDevice.get());

    if (pImageManager->getId(name) == invalid_index)
    {
        if (fs::is_ktx_format(path))
            image->create(path);
        else
            image->create(path, vk::Format::eR8G8B8A8Unorm);
    }
    
    return addImage(name, std::move(image));
}

size_t CAPIHandle::addImageAsync(const std::string& name, const std::filesystem::path& path)
{
    auto& loaderThread = EGEngine->getLoaderThread();

    std::unique_ptr<CImage> image = std::make_unique<CImage>(pDevice.get());

    if (loaderThread)
    {
        loaderThread->push([this, image = image.get(), name, path]()
            {
                if (pImageManager->getId(name) == invalid_index)
                {
                    if (fs::is_ktx_format(path))
                        image->create(path);
                    else
                        image->create(path, vk::Format::eR8G8B8A8Unorm);
                }
            });
    }
    else
    {
        if (pImageManager->getId(name) == invalid_index)
        {
            if (fs::is_ktx_format(path))
                image->create(path);
            else
                image->create(path, vk::Format::eR8G8B8A8Unorm);
        }
    }

    return addImage(name, std::move(image));
}

void CAPIHandle::incrementImageUsage(const std::string& name)
{
    pImageManager->increment(name);
}

void CAPIHandle::incrementImageUsage(size_t id)
{
    pImageManager->increment(id);
}

void CAPIHandle::removeImage(const std::string& name)
{
    pImageManager->remove(name);
}

void CAPIHandle::removeImage(size_t id)
{
    pImageManager->remove(id);
}

const std::unique_ptr<CImage>& CAPIHandle::getImage(const std::string& name)
{
    return pImageManager->get(name);
}

size_t CAPIHandle::getImageID(const std::string& name)
{
    return pImageManager->getId(name);
}

const std::unique_ptr<CImage>& CAPIHandle::getImage(size_t id)
{
    return pImageManager->get(id);
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
    return pShaderManager->add(name, std::move(shader));
}

size_t CAPIHandle::addShader(const std::string& name, const std::string& shadertype, size_t mat_id)
{
    auto shader_id = addShader(name, pLoader->load(shadertype, mat_id));

    if (mat_id != invalid_index)
    {
        auto& pMaterial = getMaterial(mat_id);
        pMaterial->setShader(shader_id);
    }

    return shader_id;
}

size_t CAPIHandle::addShader(const std::string& name, const std::string& shadertype, const FShaderSpecials& specials)
{
    auto shader_id = addShader(name, pLoader->load(shadertype, specials));
    return shader_id;
}

void CAPIHandle::removeShader(const std::string& name)
{
    pShaderManager->remove(name);
}

void CAPIHandle::removeShader(size_t id)
{
    pShaderManager->remove(id);
}

const std::unique_ptr<CShaderObject>& CAPIHandle::getShader(const std::string& name)
{
    return pShaderManager->get(name);
}

const std::unique_ptr<CShaderObject>& CAPIHandle::getShader(size_t id)
{
    return pShaderManager->get(id);
}


size_t CAPIHandle::addMaterial(const std::string& name, std::unique_ptr<CMaterial>&& material)
{
    return pMaterialManager->add(name, std::move(material));
}

void CAPIHandle::removeMaterial(const std::string& name)
{
    pMaterialManager->remove(name);
}

void CAPIHandle::removeMaterial(size_t id)
{
    pMaterialManager->remove(id);
}

const std::unique_ptr<CMaterial>& CAPIHandle::getMaterial(const std::string& name)
{
    return pMaterialManager->get(name);
}

const std::unique_ptr<CMaterial>& CAPIHandle::getMaterial(size_t id)
{
    return pMaterialManager->get(id);
}



size_t CAPIHandle::addVertexBuffer(const std::string& name)
{
    return pVertexBufferManager->add(name, std::make_unique<CVertexBufferObject>(pDevice.get()));
}

size_t CAPIHandle::addVertexBuffer(const std::string& name, std::unique_ptr<CVertexBufferObject>&& vbo)
{
    return pVertexBufferManager->add(name, std::move(vbo));
}

void CAPIHandle::removeVertexBuffer(const std::string& name)
{
    pVertexBufferManager->remove(name);
}

void CAPIHandle::removeVertexBuffer(size_t id)
{
    pVertexBufferManager->remove(id);
}

const std::unique_ptr<CVertexBufferObject>& CAPIHandle::getVertexBuffer(const std::string& name)
{
    return pVertexBufferManager->get(name);
}

const std::unique_ptr<CVertexBufferObject>& CAPIHandle::getVertexBuffer(size_t id)
{
    return pVertexBufferManager->get(id);
}


size_t CAPIHandle::addRenderStage(const std::string& name)
{
    return pRenderStageManager->add(name, std::make_unique<CRenderStage>(pDevice.get()));
}

size_t CAPIHandle::addRenderStage(const std::string& name, std::unique_ptr<CRenderStage>&& stage)
{
    return pRenderStageManager->add(name, std::move(stage));
}

void CAPIHandle::removeRenderStage(const std::string& name)
{
    pRenderStageManager->remove(name);
}

void CAPIHandle::removeRenderStage(size_t id)
{
    pRenderStageManager->remove(id);
}

const std::unique_ptr<CRenderStage>& CAPIHandle::getRenderStage(const std::string& name)
{
    return pRenderStageManager->get(name);
}

const std::unique_ptr<CRenderStage>& CAPIHandle::getRenderStage(size_t id)
{
    return pRenderStageManager->get(id);
}

size_t CAPIHandle::getRenderStageID(const std::string& name)
{
    return pRenderStageManager->getId(name);
}

const std::unique_ptr<CFramebuffer>& CAPIHandle::getFramebuffer(const std::string& srName)
{
    return getRenderStage(srName)->getFramebuffer();
}

size_t CAPIHandle::computeBRDFLUT(uint32_t size)
{
    FDispatchParam param;
    param.size = { size, size, 1u };

    auto brdfImage = std::make_unique<CImage2D>(pDevice.get());
    brdfImage->create(
        vk::Extent2D{ size, size },
        vk::Format::eR16G16Sfloat,
        vk::ImageLayout::eGeneral,
        vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment |
        vk::ImageUsageFlagBits::eStorage);

    auto output_id = addImage("brdflut", std::move(brdfImage));

    auto shader_id = addShader("brdflut_generator", "brdflut_generator");
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
    auto irradianceCubemap = std::make_unique<CImageCubemap>(pDevice.get());
    irradianceCubemap->create(
        vk::Extent2D{ size, size },
        vk::Format::eR32G32B32A32Sfloat,
        vk::ImageLayout::eGeneral,
        vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment |
        vk::ImageUsageFlagBits::eStorage);

    auto output_id = addImage("irradiance", std::move(irradianceCubemap));

    auto shader_id = addShader("irradiancecube_generator", "irradiancecube_generator");
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
    auto prefilteredCubemap = std::make_unique<CImageCubemap>(pDevice.get());
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

    auto shader_id = addShader("prefilteredmap_generator", "prefilteredmap_generator");
    auto& pShader = getShader(shader_id);
    auto& pPushConst = pShader->getPushBlock("object");

    auto cmdBuf = CCommandBuffer(pDevice.get());
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

        vk::Result res = pDevice->create(viewInfo, &levelView);
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

        pDevice->destroy(&levelView);
    }

    removeShader(shader_id);

    return output_id;
}

// Draw state machine
void CAPIHandle::bindShader(size_t id)
{
    if (id == invalid_index)
    {
        pBindedShader = nullptr;
        return;
    }

    auto& shader = getShader(id);
    if (shader)
        pBindedShader = shader.get();
    else
        log_error("Cannot bind shader, because shader with id {} is not exists!", id);
}

void CAPIHandle::bindMaterial(size_t id)
{
    if (id == invalid_index)
    {
        pBindedMaterial = nullptr;
        pBindedShader = nullptr;
        return;
    }

    auto& material = getMaterial(id);
    if (material)
    {
        pBindedMaterial = material.get();
        bindShader(pBindedMaterial->getShader());
    }
    else
        log_error("Cannot bind material, because material with id {} is not exists!", id);
}

bool CAPIHandle::bindVertexBuffer(size_t id)
{
    if (id == invalid_index)
    {
        pBindedVBO = nullptr;
        return false;
    }
    
    auto& vbo = getVertexBuffer(id);
    if (vbo && vbo->isLoaded())
    {
        auto& commandBuffer = commandBuffers->getCommandBuffer();

        pBindedVBO = vbo.get();
        pBindedVBO->bind(commandBuffer);
    }

    return vbo->isLoaded();
}

void CAPIHandle::bindRenderer(size_t id)
{
    if (id == invalid_index)
    {
        if (pBindedRenderStage)
        {
            auto& commandBuffer = commandBuffers->getCommandBuffer();

            pBindedRenderStage->end(commandBuffer);
            pBindedRenderStage = nullptr;
        }
        // Log here?

        return;
    }

    auto& renderStage = getRenderStage(id);
    if (renderStage)
    {
        auto& commandBuffer = commandBuffers->getCommandBuffer();

        pBindedRenderStage = renderStage.get();
        pBindedRenderStage->begin(commandBuffer);
    }
    else
        log_error("Cannot bind render stage, because stage with id {} is not exists!");
}

void CAPIHandle::bindTexture(const std::string& name, size_t id, uint32_t mip_level)
{
    if (pBindedShader)
        pBindedShader->addTexture(name, id, mip_level);
    else
        log_error("Cannot bind texture, cause shader was not binded.");
}

void CAPIHandle::clearQuery()
{
    auto& commandBuffer = commandBuffers->getCommandBuffer();
    pQueryPool->clear(commandBuffer);
}

void CAPIHandle::beginQuery(uint32_t index)
{
    auto& commandBuffer = commandBuffers->getCommandBuffer();
    pQueryPool->begin(commandBuffer, index);
}

void CAPIHandle::endQuery(uint32_t index)
{
    auto& commandBuffer = commandBuffers->getCommandBuffer();
    pQueryPool->end(commandBuffer, index);
}

void CAPIHandle::getQueryResult()
{
    pQueryPool->takeResult();
}

bool CAPIHandle::occlusionTest(uint32_t index)
{
    return pQueryPool->wasDrawn(index);
}

bool CAPIHandle::compareAlphaMode(EAlphaMode mode)
{
    if (pBindedMaterial)
    {
        auto& params = pBindedMaterial->getParameters();
        return params.alphaMode == mode;
    }

    log_error("Cannot compare alpha mode, cause material was not binded.");

    return false;
}

void CAPIHandle::flushConstantRanges(const std::unique_ptr<CPushHandler>& constantRange)
{
    auto& commandBuffer = commandBuffers->getCommandBuffer();
    constantRange->flush(commandBuffer);
}

void CAPIHandle::flushShader()
{
    auto& commandBuffer = commandBuffers->getCommandBuffer();

    if (pBindedShader)
        pBindedShader->predraw(commandBuffer);
    else
        log_error("Cannot flush shader data, cause shader is not binded.");
}

const std::unique_ptr<CHandler>& CAPIHandle::getUniformHandle(const std::string& name)
{
    if (pBindedShader)
        return pBindedShader->getUniformBuffer(name);

    return nullptr;
}

const std::unique_ptr<CPushHandler>& CAPIHandle::getPushBlockHandle(const std::string& name)
{
    if (pBindedShader)
        return pBindedShader->getPushBlock(name);

    return nullptr;
}

void CAPIHandle::draw(size_t begin_vertex, size_t vertex_count, size_t begin_index, size_t index_count, size_t instance_count)
{
    auto& commandBuffer = commandBuffers->getCommandBuffer();

    if (pBindedMaterial)
    {
        auto& pSurface = getUniformHandle("UBOMaterial");
        if (pSurface)
        {
            auto& params = pBindedMaterial->getParameters();

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

        auto& textures = pBindedMaterial->getTextures();
        for (auto& [name, id] : textures)
            bindTexture(name, id);
    }

    if (pBindedShader)
    {
        if(!bManualShaderControl)
            pBindedShader->predraw(commandBuffer);
    }
    else
        log_error("Shader program is not binded. Before calling draw, you should bind shader!");

    if (pBindedVBO)
    {
        auto last_index = pBindedVBO->getLastIndex();
        auto last_vertex = pBindedVBO->getLastIndex();

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
}

void CAPIHandle::dispatch(const std::vector<FDispatchParam>& params)
{
    auto& commandBuffer = commandBuffers->getCommandBuffer();

    if (pBindedShader)
        pBindedShader->dispatch(commandBuffer, params);
    else
        log_error("Cannot dispatch, cause shader was not binded.")
}

void CAPIHandle::dispatch(const FDispatchParam& param)
{
    std::vector<FDispatchParam> params{ param };
    dispatch(params);
}

vk::CommandBuffer CAPIHandle::beginFrame()
{
    bool bSwapchain{ false }, bViewport{ pDevice->isViewportResized() };

    // Here we able to recreate viewport
    if (bViewport)
    {
        reCreate(bSwapchain, bViewport);
        return nullptr;
    }

    log_cerror(!frameStarted, "Can't call beginFrame while already in progress");
    vk::Result res = pDevice->acquireNextImage(&imageIndex);
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

    frameStarted = true;

    commandBuffers->begin(vk::CommandBufferUsageFlagBits::eRenderPassContinue, imageIndex);
    return commandBuffers->getCommandBuffer();
}

vk::Result CAPIHandle::endFrame()
{
    log_cerror(frameStarted, "Can't call endFrame while frame is not in progress");
    commandBuffers->end();
    frameStarted = false;
    return commandBuffers->submit(imageIndex);
}