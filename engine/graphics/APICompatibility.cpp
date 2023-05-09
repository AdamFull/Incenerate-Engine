#include "APICompatibility.h"

#include "Device.h"

using namespace engine::graphics;

bool CAPICompatibility::checkExtensionSupport(const vk::PhysicalDevice& physicalDevice, const char* extensionName)
{
	std::vector<vk::ExtensionProperties> availableExtensions = physicalDevice.enumerateDeviceExtensionProperties();
	auto foundExtension = std::find_if(availableExtensions.begin(), availableExtensions.end(),
		[&](const vk::ExtensionProperties& extension) {
			return strcmp(extension.extensionName, extensionName) == 0;
		});
	return foundExtension != availableExtensions.end();
}

void CAPICompatibility::memoryBarrierCompat(vk::CommandBuffer& commandBuffer, const vk::MemoryBarrier2KHR& barrier2KHR)
{
	vk::DependencyInfoKHR dependencyInfo{};
	dependencyInfo.memoryBarrierCount = 1;
	dependencyInfo.pMemoryBarriers = &barrier2KHR;

	commandBuffer.pipelineBarrier2KHR(&dependencyInfo);
}

void CAPICompatibility::imageMemoryBarrierCompat(vk::CommandBuffer& commandBuffer, const vk::ImageMemoryBarrier2KHR& imageBarrier2KHR)
{
	vk::DependencyInfoKHR dependencyInfo{};
	dependencyInfo.imageMemoryBarrierCount = 1;
	dependencyInfo.pImageMemoryBarriers = &imageBarrier2KHR;

	commandBuffer.pipelineBarrier2KHR(dependencyInfo);
}

void CAPICompatibility::transitionImageLayoutTransfer(vk::CommandBuffer& commandBuffer, vk::ImageMemoryBarrier2& barrier)
{
	if (!setTransferImageLayoutFlags(barrier))
		log_error("Unsupported layout transition!");

	imageMemoryBarrierCompat(commandBuffer, barrier);
}

void CAPICompatibility::transitionImageLayoutGraphics(vk::CommandBuffer& commandBuffer, vk::ImageMemoryBarrier2& barrier)
{
	if (!setGraphicsImageLayoutFlags(barrier))
		log_error("Unsupported layout transition!");

	imageMemoryBarrierCompat(commandBuffer, barrier);
}

bool CAPICompatibility::setTransferImageLayoutFlags(vk::ImageMemoryBarrier2KHR& barrier)
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

bool CAPICompatibility::setGraphicsImageLayoutFlags(vk::ImageMemoryBarrier2KHR& barrier)
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