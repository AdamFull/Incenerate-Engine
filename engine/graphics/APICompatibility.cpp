#include "APICompatibility.h"

#include "Device.h"

using namespace engine::graphics;

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

bool APICompatibility::check(vk::Result result)
{
	return result == vk::Result::eSuccess;
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

bool APICompatibility::checkExtensionSupport(const vk::PhysicalDevice& physicalDevice, const char* extensionName)
{
	std::vector<vk::ExtensionProperties> availableExtensions = physicalDevice.enumerateDeviceExtensionProperties();
	auto foundExtension = std::find_if(availableExtensions.begin(), availableExtensions.end(),
		[&](const vk::ExtensionProperties& extension) {
			return strcmp(extension.extensionName, extensionName) == 0;
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
		VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME
	};
	
	return requiredExtensions;
}

const std::vector<const char*>& APICompatibility::getOptionalDeviceExtensions()
{
	static const std::vector<const char*> optionalExtensions{
		VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
		VK_EXT_SHADER_OBJECT_EXTENSION_NAME
	};

	return optionalExtensions;
}

std::vector<const char*> APICompatibility::getDeviceAvaliableExtensions(const vk::PhysicalDevice& physicalDevice)
{
	std::vector<const char*> allExtensions{};

	auto& requiredExtensions = getRequiredDeviceExtensions();
	for (auto& extension : requiredExtensions)
	{
		if (checkExtensionSupport(physicalDevice, extension))
			allExtensions.emplace_back(extension);
	}

	auto& optionalExtensions = getOptionalDeviceExtensions();
	for (auto& extension : optionalExtensions)
	{
		if (checkExtensionSupport(physicalDevice, extension))
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