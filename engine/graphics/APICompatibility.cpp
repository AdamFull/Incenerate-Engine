#include "APICompatibility.h"

#include "Device.h"

using namespace engine::graphics;

bool APICompatibility::bindlessSupport{ false };
bool APICompatibility::shaderObjectSupport{ false };
bool APICompatibility::timelineSemaphoreSupport{ false };

uint32_t APICompatibility::getVulkanVersion(ERenderApi eAPI)
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

void APICompatibility::printDeviceExtensions(const vk::PhysicalDevice& physicalDevice)
{
	std::vector<vk::ExtensionProperties> availableExtensions = physicalDevice.enumerateDeviceExtensionProperties();

	for (auto& extension : availableExtensions)
		log_debug(extension.extensionName);
}

bool APICompatibility::check(vk::Result result)
{
	return result == vk::Result::eSuccess;
}

std::string APICompatibility::get_error(vk::Result result)
{
	switch (result)
	{
	case vk::Result::eNotReady: return "Not ready"; break;
	case vk::Result::eTimeout: return "Timeout"; break;
	case vk::Result::eIncomplete: return "Incomplete"; break;
	case vk::Result::eErrorOutOfHostMemory: return "Out of host memory"; break;
	case vk::Result::eErrorOutOfDeviceMemory: return "Out of device memory"; break;
	case vk::Result::eErrorInitializationFailed: return "Initialization failed"; break;
	case vk::Result::eErrorDeviceLost: return "Device lost"; break;
	case vk::Result::eErrorMemoryMapFailed: return "Memory map failed"; break;
	case vk::Result::eErrorLayerNotPresent: return "Layer not present"; break;
	case vk::Result::eErrorExtensionNotPresent: return "Extension not present"; break;
	case vk::Result::eErrorFeatureNotPresent: return "Feature not present"; break;
	case vk::Result::eErrorIncompatibleDriver: return "Incompatible driver"; break;
	case vk::Result::eErrorTooManyObjects: return "Too many objects"; break;
	case vk::Result::eErrorFormatNotSupported: return "Format not supported"; break;
	case vk::Result::eErrorFragmentedPool: return "Fragment pool"; break;
	case vk::Result::eErrorUnknown: return "Unknown"; break;
	case vk::Result::eErrorOutOfPoolMemory: return "Out of pool memory"; break;
	case vk::Result::eErrorInvalidExternalHandle: return "Invalid external handle"; break;
	case vk::Result::eErrorFragmentation: return "Fragmentation"; break;
	case vk::Result::eErrorInvalidOpaqueCaptureAddress: return "Invalid opaque capture address"; break;
	case vk::Result::ePipelineCompileRequired: return "Pipeline compile required"; break;
	case vk::Result::eErrorSurfaceLostKHR: return "Surface lost"; break;
	case vk::Result::eErrorNativeWindowInUseKHR: return "Native window in use"; break;
	case vk::Result::eSuboptimalKHR: return "Suboptimal"; break;
	case vk::Result::eErrorOutOfDateKHR: return "Out of date"; break;
	case vk::Result::eErrorIncompatibleDisplayKHR: return "Incompatible display"; break;
	case vk::Result::eErrorValidationFailedEXT: return "Validation failed"; break;
	case vk::Result::eErrorInvalidShaderNV: return "Invalid shader"; break;
	case vk::Result::eErrorImageUsageNotSupportedKHR: return "Image usage not supported"; break;
	case vk::Result::eErrorVideoPictureLayoutNotSupportedKHR: return "Video picture layout not supported"; break;
	case vk::Result::eErrorVideoProfileOperationNotSupportedKHR: return "Video profile operation not supported"; break;
	case vk::Result::eErrorVideoProfileFormatNotSupportedKHR: return "Video profile format not supported"; break;
	case vk::Result::eErrorVideoProfileCodecNotSupportedKHR: return "Video profile codec not supported"; break;
	case vk::Result::eErrorVideoStdVersionNotSupportedKHR: return "Video std version not supported"; break;
	case vk::Result::eErrorInvalidDrmFormatModifierPlaneLayoutEXT: return "Invalid drm format modifier plane layout"; break;
	case vk::Result::eErrorCompressionExhaustedEXT: return "Compression exhausted"; break;
	case vk::Result::eErrorIncompatibleShaderBinaryEXT: return "Incompatible shader binary"; break;
	default: return ""; break;
	}
}

bool APICompatibility::checkValidationLayerSupport(const std::vector<const char*>& validationLayers)
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

bool APICompatibility::checkDeviceExtensionSupport(const vk::PhysicalDevice& physicalDevice, const char* extensionName)
{
	std::vector<vk::ExtensionProperties> availableExtensions = physicalDevice.enumerateDeviceExtensionProperties();
	auto foundExtension = std::find_if(availableExtensions.begin(), availableExtensions.end(),
		[&](const vk::ExtensionProperties& extension) {
			return strcmp(extension.extensionName, extensionName) == 0;
		});
	return foundExtension != availableExtensions.end();
}

bool APICompatibility::checkDeviceExtensionEnabled(const vk::PhysicalDevice& physicalDevice, const char* extensionName)
{
	auto availableExtensions = getDeviceAvaliableExtensions(physicalDevice);
	auto foundExtension = std::find_if(availableExtensions.begin(), availableExtensions.end(),
		[&](const char* extension) {
			return strcmp(extension, extensionName) == 0;
		});

	return foundExtension != availableExtensions.end();
}

bool APICompatibility::checkInstanceExtensionSupport(const char* extensionName)
{
	auto availableExtensions = vk::enumerateInstanceExtensionProperties();
	auto foundExtension = std::find_if(availableExtensions.begin(), availableExtensions.end(),
		[&](const vk::ExtensionProperties& extension) {
			return strcmp(extension.extensionName, extensionName) == 0;
		});
	return foundExtension != availableExtensions.end();
}

bool APICompatibility::checkInstanceExtensionEnabled(const char* extensionName)
{
	auto availableExtensions = getInstanceAvaliableExtensions();
	auto foundExtension = std::find_if(availableExtensions.begin(), availableExtensions.end(),
		[&](const char* extension) {
			return strcmp(extension, extensionName) == 0;
		});

	return foundExtension != availableExtensions.end();
}

vk::SurfaceFormatKHR APICompatibility::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
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

vk::PresentModeKHR APICompatibility::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes)
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

const std::vector<const char*>& APICompatibility::getRequiredDeviceExtensions()
{
	static const std::vector<const char*> requiredExtensions{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
		VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
		VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
		VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME,
		VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
		//VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME,
		VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME,
	};
	
	return requiredExtensions;
}

const std::vector<const char*>& APICompatibility::getOptionalDeviceExtensions()
{
	static const std::vector<const char*> optionalExtensions{
		VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
		VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
		VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
	};

	return optionalExtensions;
}

std::vector<const char*> APICompatibility::getDeviceAvaliableExtensions(const vk::PhysicalDevice& physicalDevice)
{
	std::vector<const char*> allExtensions{};

	auto& requiredExtensions = getRequiredDeviceExtensions();
	for (auto& extension : requiredExtensions)
	{
		if (checkDeviceExtensionSupport(physicalDevice, extension))
			allExtensions.emplace_back(extension);
	}

	auto& optionalExtensions = getOptionalDeviceExtensions();
	for (auto& extension : optionalExtensions)
	{
		if (checkDeviceExtensionSupport(physicalDevice, extension))
			allExtensions.emplace_back(extension);
	}

	return allExtensions;
}

const std::vector<const char*>& APICompatibility::getRequiredInstanceExtensions()
{
	static const std::vector<const char*> requiredExtensions{
	};

	return requiredExtensions;
}

const std::vector<const char*>& APICompatibility::getOptionalInstanceExtensions()
{
	static const std::vector<const char*> optionalExtensions{
#ifdef _DEBUG
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
		VK_EXT_DEBUG_MARKER_EXTENSION_NAME,
#endif
	};

	return optionalExtensions;
}

std::vector<const char*> APICompatibility::getInstanceAvaliableExtensions()
{
	std::vector<const char*> allExtensions{};

	auto& requiredExtensions = getRequiredInstanceExtensions();
	for (auto& extension : requiredExtensions)
	{
		if (checkInstanceExtensionSupport(extension))
			allExtensions.emplace_back(extension);
	}

	auto& optionalExtensions = getOptionalInstanceExtensions();
	for (auto& extension : optionalExtensions)
	{
		if (checkInstanceExtensionSupport(extension))
			allExtensions.emplace_back(extension);
	}

	return allExtensions;
}

void APICompatibility::memoryBarrierCompat(vk::CommandBuffer& commandBuffer, const vk::MemoryBarrier2KHR& barrier2KHR)
{
	vk::DependencyInfoKHR dependencyInfo{};
	dependencyInfo.memoryBarrierCount = 1;
	dependencyInfo.pMemoryBarriers = &barrier2KHR;

	commandBuffer.pipelineBarrier2KHR(&dependencyInfo);
}

void APICompatibility::imageMemoryBarrierCompat(vk::CommandBuffer& commandBuffer, const vk::ImageMemoryBarrier2KHR& imageBarrier2KHR)
{
	vk::DependencyInfoKHR dependencyInfo{};
	dependencyInfo.imageMemoryBarrierCount = 1;
	dependencyInfo.pImageMemoryBarriers = &imageBarrier2KHR;

	commandBuffer.pipelineBarrier2KHR(dependencyInfo);
}

void APICompatibility::transitionImageLayoutTransfer(vk::CommandBuffer& commandBuffer, vk::ImageMemoryBarrier2& barrier)
{
	if (!setTransferImageLayoutFlags(barrier))
		log_error("Unsupported layout transition!");

	imageMemoryBarrierCompat(commandBuffer, barrier);
}

void APICompatibility::transitionImageLayoutGraphics(vk::CommandBuffer& commandBuffer, vk::ImageMemoryBarrier2& barrier)
{
	if (!setGraphicsImageLayoutFlags(barrier))
		log_error("Unsupported layout transition!");

	imageMemoryBarrierCompat(commandBuffer, barrier);
}

void APICompatibility::applyDescriptorIndexingFeatures(vk::PhysicalDeviceVulkan12Features& features)
{
	features.descriptorIndexing =
	features.descriptorBindingPartiallyBound = 
	features.runtimeDescriptorArray = 
	features.descriptorBindingSampledImageUpdateAfterBind = 
	features.descriptorBindingVariableDescriptorCount = bindlessSupport;
}

void APICompatibility::applyDescriptorIndexingFeatures(vk::PhysicalDeviceDescriptorIndexingFeatures& features)
{
	features.descriptorBindingPartiallyBound =
	features.runtimeDescriptorArray =
	features.descriptorBindingSampledImageUpdateAfterBind =
	features.descriptorBindingVariableDescriptorCount = bindlessSupport;
}

void APICompatibility::applyTimelineSemaphoreFeatures(vk::PhysicalDeviceVulkan12Features& features)
{
	features.timelineSemaphore = timelineSemaphoreSupport;
}

void APICompatibility::applyTimelineSemaphoreFeatures(vk::PhysicalDeviceTimelineSemaphoreFeaturesKHR& features)
{
	features.timelineSemaphore = timelineSemaphoreSupport;
}

bool APICompatibility::setTransferImageLayoutFlags(vk::ImageMemoryBarrier2KHR& barrier)
{
	if (barrier.oldLayout == vk::ImageLayout::eUndefined && barrier.newLayout == vk::ImageLayout::eGeneral)
	{
		barrier.srcAccessMask = (vk::AccessFlagBits2)0;
		barrier.dstAccessMask = vk::AccessFlagBits2::eTransferWrite;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
	}
	else if (barrier.oldLayout == vk::ImageLayout::eUndefined && barrier.newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.srcAccessMask = (vk::AccessFlagBits2)0;
		barrier.dstAccessMask = vk::AccessFlagBits2::eTransferWrite;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
	}
	else if (barrier.oldLayout == vk::ImageLayout::eTransferDstOptimal && barrier.newLayout == vk::ImageLayout::eTransferSrcOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits2::eTransferRead;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
	}
	else if (barrier.oldLayout == vk::ImageLayout::eTransferDstOptimal && barrier.newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits2::eTransferWrite;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
	}
	else if (barrier.oldLayout == vk::ImageLayout::eTransferSrcOptimal && barrier.newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits2::eTransferRead;
		barrier.dstAccessMask = vk::AccessFlagBits2::eTransferWrite;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
	}
	else if (barrier.oldLayout == vk::ImageLayout::eTransferDstOptimal && barrier.newLayout == vk::ImageLayout::eGeneral)
	{
		barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits2::eTransferWrite | vk::AccessFlagBits2::eTransferRead;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
	}
	else if (barrier.oldLayout == vk::ImageLayout::eGeneral && barrier.newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite | vk::AccessFlagBits2::eTransferRead;
		barrier.dstAccessMask = vk::AccessFlagBits2::eTransferWrite;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
	}
	else if (barrier.oldLayout == vk::ImageLayout::eTransferSrcOptimal && barrier.newLayout == vk::ImageLayout::eGeneral)
	{
		barrier.srcAccessMask = vk::AccessFlagBits2::eTransferRead;
		barrier.dstAccessMask = vk::AccessFlagBits2::eTransferWrite | vk::AccessFlagBits2::eTransferRead;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
	}
	else if (barrier.oldLayout == vk::ImageLayout::eGeneral && barrier.newLayout == vk::ImageLayout::eTransferSrcOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite | vk::AccessFlagBits2::eTransferRead;
		barrier.dstAccessMask = vk::AccessFlagBits2::eTransferRead;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
	}
	else
		return false;

	return true;
}

bool APICompatibility::setGraphicsImageLayoutFlags(vk::ImageMemoryBarrier2KHR& barrier)
{
	if (barrier.oldLayout == vk::ImageLayout::eUndefined && barrier.newLayout == vk::ImageLayout::eGeneral)
	{
		barrier.srcAccessMask = (vk::AccessFlagBits2)0;
		barrier.dstAccessMask = vk::AccessFlagBits2::eTransferWrite;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
	}
	else if (barrier.oldLayout == vk::ImageLayout::eUndefined && barrier.newLayout == vk::ImageLayout::eColorAttachmentOptimal)
	{
		barrier.srcAccessMask = (vk::AccessFlagBits2)0;
		barrier.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
	}
	else if (barrier.oldLayout == vk::ImageLayout::eUndefined && barrier.newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		barrier.srcAccessMask = (vk::AccessFlagBits2)0;
		barrier.dstAccessMask = vk::AccessFlagBits2::eDepthStencilAttachmentRead | vk::AccessFlagBits2::eDepthStencilAttachmentWrite;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests;
	}
	else if (barrier.oldLayout == vk::ImageLayout::eUndefined && barrier.newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.srcAccessMask = (vk::AccessFlagBits2)0;
		barrier.dstAccessMask = vk::AccessFlagBits2::eShaderRead;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader;
	}
	else if (barrier.oldLayout == vk::ImageLayout::eTransferSrcOptimal && barrier.newLayout == vk::ImageLayout::eColorAttachmentOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
	}
	else if (barrier.oldLayout == vk::ImageLayout::eTransferDstOptimal && barrier.newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits2::eShaderRead;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader;
	}
	else if (barrier.oldLayout == vk::ImageLayout::eTransferSrcOptimal && barrier.newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits2::eShaderRead;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader;
	}
	else if (barrier.oldLayout == vk::ImageLayout::eGeneral && barrier.newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits2::eShaderRead;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader;
	}
	else if (barrier.oldLayout == vk::ImageLayout::ePresentSrcKHR && barrier.newLayout == vk::ImageLayout::eTransferSrcOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits2::eMemoryRead;
		barrier.dstAccessMask = vk::AccessFlagBits2::eTransferRead;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
	}
	else if (barrier.oldLayout == vk::ImageLayout::eTransferSrcOptimal && barrier.newLayout == vk::ImageLayout::ePresentSrcKHR)
	{
		barrier.srcAccessMask = vk::AccessFlagBits2::eTransferRead;
		barrier.dstAccessMask = vk::AccessFlagBits2::eMemoryRead;

		barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
		barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;
	}
	else
		return false;

	return true;
}