#include "Helpers.h"

#include <set>

#include "Engine.h"
#include "system/window/WindowHandle.h"

#include <SDL_vulkan.h>

using namespace engine;
using namespace engine::graphics;
using namespace engine::system::window;
using namespace engine::graphics;

bool VkHelper::check(vk::Result result)
{
    return result == vk::Result::eSuccess;
}

bool VkHelper::checkValidationLayerSupport(const std::vector<const char*>& validationLayers)
{
    auto availableLayers = vk::enumerateInstanceLayerProperties();

    log_debug("Enabled validation layers:");
    bool layerFound{ false };
    for (auto& layerName : validationLayers)
    {
        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                log_debug(layerProperties.layerName);
                layerFound = true;
                break;
            }
        }
    }

    return layerFound;
}

std::vector<const char*> VkHelper::getRequiredExtensions(bool validation)
{
    auto window = CEngine::getInstance()->getWindow()->getWindowPointer();

    uint32_t extensionCount{ 0 };
    std::vector<const char*> extensions{};

    if (SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr))
    {
        extensions.resize(extensionCount);
        SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions.data());
    }

    if (validation)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

bool VkHelper::checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const std::vector<const char*>& deviceExtensions)
{
    std::set<std::string> sRequiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : device.enumerateDeviceExtensionProperties())
    {
        sRequiredExtensions.erase(extension.extensionName);
    }

    return sRequiredExtensions.empty();
}

vk::SurfaceFormatKHR VkHelper::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
    if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
    {
        return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
    }

    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

vk::PresentModeKHR VkHelper::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes)
{
    vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox)
        {
            return availablePresentMode;
        }
        else if (availablePresentMode == vk::PresentModeKHR::eImmediate)
        {
            bestMode = availablePresentMode;
        }
    }

    return bestMode;
}

bool VkHelper::hasStencilComponent(vk::Format format)
{
    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

void VkHelper::BarrierFromComputeToCompute()
{
    auto& graphics = EGEngine->getGraphics();
    auto commandBuffer = graphics->getCommandBuffer();
    BarrierFromComputeToCompute(commandBuffer);
}

void VkHelper::BarrierFromComputeToCompute(vk::CommandBuffer& commandBuffer)
{
    vk::MemoryBarrier2KHR barrier{};
    barrier.srcStageMask = vk::PipelineStageFlagBits2::eComputeShader;
    barrier.srcAccessMask = vk::AccessFlagBits2::eShaderWrite;
    barrier.dstStageMask = vk::PipelineStageFlagBits2::eComputeShader;
    barrier.dstAccessMask = vk::AccessFlagBits2::eShaderRead;

    vk::DependencyInfoKHR dependencyInfo{};
    dependencyInfo.memoryBarrierCount = 1;
    dependencyInfo.pMemoryBarriers = &barrier;

    commandBuffer.pipelineBarrier2KHR(&dependencyInfo);
}

void VkHelper::BarrierFromComputeToGraphics()
{
    auto& graphics = EGEngine->getGraphics();
    auto commandBuffer = graphics->getCommandBuffer();
    BarrierFromComputeToGraphics(commandBuffer);
}

void VkHelper::BarrierFromComputeToGraphics(vk::CommandBuffer& commandBuffer)
{
    vk::MemoryBarrier2KHR barrier{};
    barrier.srcStageMask = vk::PipelineStageFlagBits2::eComputeShader;
    barrier.srcAccessMask = vk::AccessFlagBits2::eShaderWrite;
    barrier.dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader;
    barrier.dstAccessMask = vk::AccessFlagBits2::eShaderRead;

    vk::DependencyInfoKHR dependencyInfo{};
    dependencyInfo.memoryBarrierCount = 1;
    dependencyInfo.pMemoryBarriers = &barrier;

    commandBuffer.pipelineBarrier2KHR(&dependencyInfo);
}

void VkHelper::BarrierFromGraphicsToCompute(size_t image_id)
{
    auto& graphics = EGEngine->getGraphics();
    auto commandBuffer = graphics->getCommandBuffer();
    BarrierFromGraphicsToCompute(commandBuffer, image_id);
}

void VkHelper::BarrierFromGraphicsToCompute(vk::CommandBuffer& commandBuffer, size_t image_id)
{
    auto& graphics = EGEngine->getGraphics();

    if (image_id != invalid_index)
    {
        auto& image = graphics->getImage(image_id);

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

        vk::DependencyInfoKHR dependencyInfo{};
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &imageMemoryBarrier;

        commandBuffer.pipelineBarrier2KHR(dependencyInfo);
    }
}

void VkHelper::BarrierFromGraphicsToTransfer(size_t image_id)
{
    auto& graphics = EGEngine->getGraphics();
    auto commandBuffer = graphics->getCommandBuffer();
    BarrierFromGraphicsToTransfer(commandBuffer, image_id);
}

void VkHelper::BarrierFromGraphicsToTransfer(vk::CommandBuffer& commandBuffer, size_t image_id)
{
    auto& graphics = EGEngine->getGraphics();

    if (image_id != invalid_index)
    {
        auto& image = graphics->getImage(image_id);

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

        vk::DependencyInfoKHR dependencyInfo{};
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &imageMemoryBarrier;

        commandBuffer.pipelineBarrier2KHR(dependencyInfo);
    }
}