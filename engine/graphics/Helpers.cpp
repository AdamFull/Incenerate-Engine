#include "Helpers.h"

#include <set>

#include "Engine.h"
#include "system/window/WindowHandle.h"

#include <utility/ulog.hpp>

#include <SDL_vulkan.h>

using namespace engine;
using namespace engine::system::window;
using namespace engine::graphics;

bool VulkanStaticHelper::checkValidationLayerSupport(const std::vector<const char*>& validationLayers)
{
    auto availableLayers = vk::enumerateInstanceLayerProperties();

    for (auto& layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            utl::log<utl::level::eWarning>(layerProperties.layerName);
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

std::vector<const char*> VulkanStaticHelper::getRequiredExtensions(bool validation)
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

bool VulkanStaticHelper::checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const std::vector<const char*>& deviceExtensions)
{
    std::set<std::string> sRequiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : device.enumerateDeviceExtensionProperties())
    {
        sRequiredExtensions.erase(extension.extensionName);
    }

    return sRequiredExtensions.empty();
}

vk::SurfaceFormatKHR VulkanStaticHelper::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
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

vk::PresentModeKHR VulkanStaticHelper::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes)
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

bool VulkanStaticHelper::hasStencilComponent(vk::Format format)
{
    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}
