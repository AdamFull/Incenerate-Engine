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

void CAPIHandle::create(const FEngineCreateInfo& createInfo)
{
	eAPI = createInfo.eAPI;

	pDevice = std::make_unique<CDevice>(this);
	pDevice->create(createInfo);

    pLoader = std::make_unique<CShaderLoader>(pDevice.get());
    pLoader->create(createInfo.srShaders);

    pResourceHolder = std::make_unique<CResourceHolder>();

    screenExtent = pDevice->getExtent();
    commandBuffers = std::make_unique<CCommandBuffer>(pDevice.get());
    commandBuffers->create(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, pDevice->getFramesInFlight());

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
    EGCoordinator->sendEvent(Events::Graphics::ReCreate);
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

const std::unique_ptr<CDevice>& CAPIHandle::getDevice() const
{
    return pDevice;
}

const std::unique_ptr<CShaderLoader>& CAPIHandle::getShaderLoader()
{
    return pLoader;
}

const std::unique_ptr<CResourceHolder>& CAPIHandle::getResourceHolder()
{
    return pResourceHolder;
}

std::unique_ptr<CVertexBufferObject> CAPIHandle::allocateVBO()
{
    return std::make_unique<CVertexBufferObject>(pDevice.get());
}

const std::unique_ptr<CFramebuffer>& CAPIHandle::getFramebuffer(const std::string& srName)
{
    return getRenderStage(srName)->getFramebuffer();
}

const std::unique_ptr<CShaderObject>& CAPIHandle::getShader(size_t id)
{
    return pResourceHolder->getShader(id);
}

const std::unique_ptr<CImage>& CAPIHandle::getImage(size_t id)
{
    return pResourceHolder->getImage(id);
}

const std::unique_ptr<CMaterial>& CAPIHandle::getMaterial(size_t id)
{
    return pResourceHolder->getMaterial(id);
}

const std::unique_ptr<CVertexBufferObject>& CAPIHandle::getVertexBuffer(size_t id)
{
    return pResourceHolder->getVertexBuffer(id);
}

const std::unique_ptr<CRenderStage>& CAPIHandle::getRenderStage(size_t id)
{
    return pResourceHolder->getRenderStage(id);
}

const std::unique_ptr<CRenderStage>& CAPIHandle::getRenderStage(const std::string& srName)
{
    return getRenderStage(mStages[srName]);
}

size_t CAPIHandle::createRenderStage(const std::string& srName)
{
    auto id = pResourceHolder->addRenderStage(std::make_unique<CRenderStage>(pDevice.get()));
    mStages.emplace(srName, id);
    return id;
}

size_t CAPIHandle::createShader(const std::string& srName)
{
    return pResourceHolder->addShader(pLoader->load(srName));
}

size_t CAPIHandle::createImage(const std::string& srPath)
{
    auto pImage = std::make_unique<CImage>(pDevice.get());
    pImage->create(srPath);
    return pResourceHolder->addImage(std::move(pImage));
}

size_t CAPIHandle::createMaterial(const std::string& srName)
{
    return invalid_index;
}

size_t CAPIHandle::createVBO()
{
    auto pVBO = std::make_unique<CVertexBufferObject>(pDevice.get());
    return pResourceHolder->addVertexBuffer(std::move(pVBO));
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