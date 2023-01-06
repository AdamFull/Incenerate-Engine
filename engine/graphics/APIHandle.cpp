#include "APIHandle.h"

#include "system/window/WindowHandle.h"
#include "Engine.h"

using namespace engine::graphics;
using namespace engine::ecs;
using namespace engine::system::window;

CAPIHandle::CAPIHandle(winhandle_t window)
{
    pWindow = window;
}

CAPIHandle::~CAPIHandle()
{
    pRenderStageManager = nullptr;
    pVertexBufferManager = nullptr;
    pMaterialManager = nullptr;
    pShaderManager = nullptr;
    pImageManager = nullptr;
    pDevice = nullptr;
    pLoader = nullptr;
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
    pLoader->create(createInfo.srShaders);

    screenExtent = pDevice->getExtent();
    commandBuffers = std::make_unique<CCommandBuffer>(pDevice.get());
    commandBuffers->create(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, pDevice->getFramesInFlight());

    {
        mStageInfos["deferred"].srName = "deferred";
        mStageInfos["deferred"].viewport.offset = vk::Offset2D(0, 0);
        mStageInfos["deferred"].viewport.extent = EGGraphics->getDevice()->getExtent();
        mStageInfos["deferred"].bFlipViewport = true;
        mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "packed_tex", vk::Format::eR32G32B32A32Uint, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
        mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "emission_tex", vk::Format::eB10G11R11UfloatPack32, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
        mStageInfos["deferred"].vImages.emplace_back(FCIImage{ "depth_tex", EGGraphics->getDevice()->getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled });
        mStageInfos["deferred"].vOutputs.emplace_back("packed_tex");
        mStageInfos["deferred"].vOutputs.emplace_back("emission_tex");
        mStageInfos["deferred"].vDescriptions.emplace_back("depth_tex");
        mStageInfos["deferred"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eBottomOfPipe,
                    vk::AccessFlags{}
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
        mStageInfos["composition"].viewport.extent = EGGraphics->getDevice()->getExtent();
        //stageCI.bFlipViewport = true;
        mStageInfos["composition"].vImages.emplace_back(FCIImage{ "present_khr", EGGraphics->getDevice()->getImageFormat(), vk::ImageUsageFlagBits::eColorAttachment });
        mStageInfos["composition"].vOutputs.emplace_back("present_khr");
        mStageInfos["composition"].vDescriptions.emplace_back("");
        mStageInfos["composition"].vDependencies.emplace_back(
            FCIDependency(
                FCIDependencyDesc(
                    VK_SUBPASS_EXTERNAL,
                    vk::PipelineStageFlagBits::eAllCommands,
                    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite
                ),
                FCIDependencyDesc(
                    0,
                    vk::PipelineStageFlagBits::eAllGraphics,
                    vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite
                )
            )
        );

        auto stageId = EGGraphics->addRenderStage("composition");
        auto& pStage = EGGraphics->getRenderStage(stageId);
        pStage->create(mStageInfos["composition"]);
    }
    

    log_info("Graphics core initialized.");
}

void CAPIHandle::reCreate()
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

    while (pWindow->isMinimized())
        pWindow->begin();

    pDevice->GPUWait();

    pDevice->recreateSwapchain();
    screenExtent = pDevice->getExtent();
    imageIndex = 0;
    CWindowHandle::bWasResized = false;
    for (auto& [name, stage] : mStageInfos)
    {
        stage.viewport.extent = pDevice->getExtent();
        auto& pStage = pRenderStageManager->get(name);
        pStage->reCreate(stage);
    }
}

void CAPIHandle::shutdown()
{
    pDevice->GPUWait();
}

vk::CommandBuffer CAPIHandle::begin()
{
    vk::CommandBuffer commandBuffer{};
    try { commandBuffer = beginFrame(); }
    catch (vk::OutOfDateKHRError err) { reCreate(); }
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
        reCreate();

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

size_t CAPIHandle::addImage(const std::string& name, const std::string& path)
{
    auto image = std::make_unique<CImage>(pDevice.get());
    image->create(path);
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

vk::CommandBuffer CAPIHandle::beginFrame()
{
    log_cerror(!frameStarted, "Can't call beginFrame while already in progress");
    vk::Result res = pDevice->acquireNextImage(&imageIndex);
    if (res == vk::Result::eErrorOutOfDateKHR)
    {
        reCreate();
        return nullptr;
    }
    else if (res == vk::Result::eSuboptimalKHR) {
        log_error("Failed to acquire swap chain image!");
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