#include "APIHandle.h"

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

	pDevice = std::make_unique<CDevice>();
	pDevice->create(createInfo);

    pLoader = std::make_unique<CShaderLoader>(pDevice.get());

    screenExtent = pDevice->getExtent();
    commandBuffers = std::make_unique<CCommandBuffer>(pDevice.get());
    commandBuffers->create(false, vk::QueueFlagBits::eGraphics, vk::CommandBufferLevel::ePrimary, pDevice->getFramesInFlight());
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

    while (pWindow->isMinimized())
        pWindow->begin();

    pDevice->GPUWait();

    pDevice->tryRebuildSwapchain();
    screenExtent = pDevice->getExtent();
    imageIndex = 0;
    CWindowHandle::bWasResized = false;

    for (auto& stage : vStages)
        stage->reCreate();
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
    catch (vk::SystemError err) { throw std::runtime_error("Failed to acquire swap chain image!"); }

    if (!commandBuffer)
        return;
    
    //TODO: select between drawcall and dispatch
    for (auto& stage : vStages)
        stage->render(commandBuffer);

    vk::Result resultPresent;
    try { resultPresent = endFrame(); }
    catch (vk::OutOfDateKHRError err) { resultPresent = vk::Result::eErrorOutOfDateKHR; }
    catch (vk::SystemError err) { throw std::runtime_error("failed to present swap chain image!"); }

    if (resultPresent == vk::Result::eSuboptimalKHR || resultPresent == vk::Result::eErrorOutOfDateKHR || CWindowHandle::bWasResized)
        reCreate();

    pDevice->updateCommandPools();
}

const std::unique_ptr<CShaderObject>& CAPIHandle::addStage(const std::string& shader_name)
{
    vStages.emplace_back(pLoader->load(shader_name));
    return vStages.back();
}

const std::unique_ptr<CDevice>& CAPIHandle::getDevice() const
{
    return pDevice;
}

const std::unique_ptr<CShaderLoader>& CAPIHandle::getShaderLoader()
{
    return pLoader;
}

std::unique_ptr<CVertexBufferObject> CAPIHandle::allocateVBO()
{
    return std::make_unique<CVertexBufferObject>(pDevice.get());
}

vk::CommandBuffer CAPIHandle::beginFrame()
{
    assert(!frameStarted && "Can't call beginFrame while already in progress");
    vk::Result res = pDevice->acquireNextImage(&imageIndex);
    if (res == vk::Result::eErrorOutOfDateKHR)
    {
        reCreate();
        return nullptr;
    }
    else if (res == vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    frameStarted = true;

    commandBuffers->begin(vk::CommandBufferUsageFlagBits::eRenderPassContinue, imageIndex);
    return commandBuffers->getCommandBuffer();
}

vk::Result CAPIHandle::endFrame()
{
    assert(frameStarted && "Can't call endFrame while frame is not in progress");
    commandBuffers->end();
    frameStarted = false;
    return commandBuffers->submit(imageIndex);
}