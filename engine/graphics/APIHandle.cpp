#include "APIHandle.h"

#include "system/window/WindowHandle.h"
#include "system/filesystem/filesystem.h"
#include "Engine.h"

#include "image/Image2D.h"
#include "image/ImageCubemap.h"

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

    auto& device = EGGraphics->getDevice();
    auto depth_format = device->getDepthFormat();
    

    {
        mStageInfos["shadow"].srName = "shadow";
        mStageInfos["shadow"].viewport.offset = vk::Offset2D(0, 0);
        mStageInfos["shadow"].viewport.extent = vk::Extent2D(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
        mStageInfos["shadow"].bIgnoreRecreation = true;
        mStageInfos["shadow"].bFlipViewport = false;
        mStageInfos["shadow"].vImages.emplace_back("direct_shadowmap_tex", depth_format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, EImageType::eArray2D, MAX_SPOT_LIGHT_COUNT);
        mStageInfos["shadow"].vDescriptions.emplace_back("direct_shadowmap_tex");
        mStageInfos["shadow"].vImages.emplace_back("omni_shadowmap_tex", depth_format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, EImageType::eArrayCube, MAX_POINT_LIGHT_COUNT);
        mStageInfos["shadow"].vDescriptions.emplace_back("omni_shadowmap_tex");
        mStageInfos["shadow"].vDependencies.emplace_back(
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
        mStageInfos["shadow"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    1,
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
        mStageInfos["shadow"].vDependencies.emplace_back(
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

        auto stageId = EGGraphics->addRenderStage("shadow");
        auto& pStage = EGGraphics->getRenderStage(stageId);
        pStage->create(mStageInfos["shadow"]);
    }

    {
        mStageInfos["deferred"].srName = "deferred";
        mStageInfos["deferred"].viewport.offset = vk::Offset2D(0, 0);
        mStageInfos["deferred"].viewport.extent = device->getExtent(true);
        mStageInfos["deferred"].bFlipViewport = true;
        mStageInfos["deferred"].bViewportDependent = true;
        mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "albedo_tex", vk::Format::eR8G8B8A8Srgb, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
        mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "normal_tex", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
        mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "mrah_tex", vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
        mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "emission_tex", vk::Format::eR16G16B16A16Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
        mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "depth_tex", depth_format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled });
        mStageInfos["deferred"].vOutputs.emplace_back("albedo_tex");
        mStageInfos["deferred"].vOutputs.emplace_back("normal_tex");
        mStageInfos["deferred"].vOutputs.emplace_back("mrah_tex");
        mStageInfos["deferred"].vOutputs.emplace_back("emission_tex");
        mStageInfos["deferred"].vDescriptions.emplace_back("depth_tex");
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

        auto stageId = EGGraphics->addRenderStage("deferred");
        auto& pStage = EGGraphics->getRenderStage(stageId);
        pStage->create(mStageInfos["deferred"]);
    }

    {
        mStageInfos["composition"].srName = "composition";
        mStageInfos["composition"].viewport.offset = vk::Offset2D(0, 0);
        mStageInfos["composition"].viewport.extent = device->getExtent(true);
        mStageInfos["composition"].bViewportDependent = true;
        mStageInfos["composition"].vImages.emplace_back(FCIImage{ "composition_tex", vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
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

        auto stageId = EGGraphics->addRenderStage("composition");
        auto& pStage = EGGraphics->getRenderStage(stageId);
        pStage->create(mStageInfos["composition"]);
    }

    {
        mStageInfos["tonemapping"].srName = "tonemapping";
        mStageInfos["tonemapping"].viewport.offset = vk::Offset2D(0, 0);
        mStageInfos["tonemapping"].viewport.extent = device->getExtent(true);
        mStageInfos["tonemapping"].bViewportDependent = true;
        mStageInfos["tonemapping"].vImages.emplace_back(FCIImage{ "postprocess_tex", vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
        mStageInfos["tonemapping"].vImages.emplace_back(FCIImage{ "brightness_tex", vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
        mStageInfos["tonemapping"].vOutputs.emplace_back("postprocess_tex");
        mStageInfos["tonemapping"].vOutputs.emplace_back("brightness_tex");
        mStageInfos["tonemapping"].vDescriptions.emplace_back("");
        mStageInfos["tonemapping"].vDependencies.emplace_back(
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
        mStageInfos["tonemapping"].vDependencies.emplace_back(
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

        auto stageId = EGGraphics->addRenderStage("tonemapping");
        auto& pStage = EGGraphics->getRenderStage(stageId);
        pStage->create(mStageInfos["tonemapping"]);
    }

    {
        mStageInfos["postprocess"].srName = "postprocess";
        mStageInfos["postprocess"].viewport.offset = vk::Offset2D(0, 0);
        mStageInfos["postprocess"].viewport.extent = device->getExtent(true);
        mStageInfos["postprocess"].vDescriptions.emplace_back("");
        mStageInfos["postprocess"].vDependencies.emplace_back(
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
        mStageInfos["postprocess"].vDependencies.emplace_back(
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

        auto stageId = EGGraphics->addRenderStage("postprocess");
        auto& pStage = EGGraphics->getRenderStage(stageId);
        pStage->create(mStageInfos["postprocess"]);
    }
    
    {
        mStageInfos["present"].srName = "present";
        mStageInfos["present"].viewport.offset = vk::Offset2D(0, 0);
        mStageInfos["present"].viewport.extent = device->getExtent();
        mStageInfos["present"].vImages.emplace_back(FCIImage{ "present_khr", device->getImageFormat(), vk::ImageUsageFlagBits::eColorAttachment });
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

        auto stageId = EGGraphics->addRenderStage("present");
        auto& pStage = EGGraphics->getRenderStage(stageId);
        pStage->create(mStageInfos["present"]);
    }

    log_info("Graphics core initialized.");
}

void CAPIHandle::update()
{
    if (imageIndex == pDevice->getFramesInFlight() - 1)
    {
        pImageManager->perform_deletion();
        pShaderManager->perform_deletion();
        pMaterialManager->perform_deletion();
        pVertexBufferManager->perform_deletion();
        pRenderStageManager->perform_deletion();
    }
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
    pRenderStageManager = nullptr;
    pMaterialManager = nullptr;
    pVertexBufferManager = nullptr;
    pShaderManager = nullptr;
    pImageManager = nullptr;
}

vk::CommandBuffer CAPIHandle::begin()
{
    vk::CommandBuffer commandBuffer{};
    try { commandBuffer = beginFrame(); }
    catch (vk::OutOfDateKHRError err) { reCreate(true, true); }
    catch (vk::SystemError err) { log_error("Failed to acquire swap chain image!"); }
    return commandBuffer;
}

void CAPIHandle::end()
{
    vk::Result resultPresent;
    try { resultPresent = endFrame(); }
    catch (vk::OutOfDateKHRError err) { resultPresent = vk::Result::eErrorOutOfDateKHR; }
    catch (vk::SystemError err) { log_error("failed to present swap chain image!"); }

    if (resultPresent == vk::Result::eSuboptimalKHR || resultPresent == vk::Result::eErrorOutOfDateKHR || CWindowHandle::bWasResized)
        reCreate(true, true);

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
    
     if (pImageManager->get_id(name) == invalid_index)
     {
         if (fs::is_ktx_format(path))
             image->create(path);
         else
             image->create(path, vk::Format::eR8G8B8A8Unorm);
     }
    
    
    return addImage(name, std::move(image));
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
    return pImageManager->get_id(name);
}

const std::unique_ptr<CImage>& CAPIHandle::getImage(size_t id)
{
    return pImageManager->get(id);
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


const std::unique_ptr<CFramebuffer>& CAPIHandle::getFramebuffer(const std::string& srName)
{
    return getRenderStage(srName)->getFramebuffer();
}

size_t CAPIHandle::computeBRDFLUT(uint32_t size)
{
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

    pShader->dispatch({ size, size });

    removeShader(shader_id);

    return output_id;
}

size_t CAPIHandle::computeIrradiance(size_t origin, uint32_t size)
{
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
    pShader->addTexture("samplerColour", origin);

    pShader->dispatch({ size, size });

    removeShader(shader_id);

    return output_id;
}

size_t CAPIHandle::computePrefiltered(size_t origin, uint32_t size)
{
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
        pShader->addTexture("samplerColour", origin);

        pShader->dispatch(commandBuffer, { size, size });
        cmdBuf.submitIdle();

        pDevice->destroy(&levelView);
    }

    removeShader(shader_id);

    return output_id;
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