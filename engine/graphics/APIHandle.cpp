#include "APIHandle.h"

#include "graphics/rendering/RenderSystem.h"
#include "system/window/WindowHandle.h"

using namespace engine::graphics;
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

    screenExtent = pDevice->getExtent();
    commandBuffers = std::make_unique<CCommandBuffer>(pDevice.get());
    commandBuffers->create(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, pDevice->getFramesInFlight());

    pRenderSystem = std::make_unique<CRenderSystem>(pDevice.get());
    pRenderSystem->create("rendering.json");

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
    pRenderSystem->reCreate();
}

void CAPIHandle::shutdown()
{
    pDevice->GPUWait();
}

void CAPIHandle::render()
{
    vk::CommandBuffer commandBuffer{};
    try { commandBuffer = beginFrame(); }
    catch (vk::OutOfDateKHRError err) { reCreate(); }
    catch (vk::SystemError err) { log_error("Failed to acquire swap chain image!"); }

    if (!commandBuffer)
        return;
    
    pRenderSystem->render(commandBuffer);

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

const std::unique_ptr<CRenderSystem>& CAPIHandle::getRenderSystem()
{
    return pRenderSystem;
}

std::unique_ptr<CVertexBufferObject> CAPIHandle::allocateVBO()
{
    return std::make_unique<CVertexBufferObject>(pDevice.get());
}

const std::unique_ptr<CFramebuffer>& CAPIHandle::getFramebuffer(const std::string& srName)
{
    auto& pStage = pRenderSystem->getStage(srName);
    return pStage->getFramebuffer();
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