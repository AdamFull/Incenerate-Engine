#include "Helpers.h"

#include <set>

#include "Engine.h"

using namespace engine;
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

uint32_t VkHelper::getVulkanVersion(ERenderApi eAPI)
{
    switch (eAPI)
    {
    case ERenderApi::eVulkan_1_0:
        return VK_API_VERSION_1_0;
    case ERenderApi::eVulkan_1_1:
        return VK_API_VERSION_1_1;
    case ERenderApi::eVulkan_1_2:
        return VK_API_VERSION_1_2;
    case ERenderApi::eVulkan_1_3:
        return VK_API_VERSION_1_3;
    }

    return 0;
}