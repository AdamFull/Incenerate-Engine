#include "Helpers.h"

#include <set>

#include "Engine.h"
#include "system/window/WindowHandle.h"

#include <SDL3/SDL_vulkan.h>

using namespace engine;
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
