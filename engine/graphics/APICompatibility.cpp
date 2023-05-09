#include "APICompatibility.h"

using namespace engine::graphics;

CAPICompatibility::CAPICompatibility(uint32_t vkVersion) :
	m_vkVersion(vkVersion)
{

}

void CAPICompatibility::applyPhysicalDevice(const vk::PhysicalDevice& physicalDevice)
{
	m_vDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
}

bool CAPICompatibility::checkDeviceExtensionSupport(const char* extensionName)
{
	auto foundExtension = std::find_if(m_vDeviceExtensions.begin(), m_vDeviceExtensions.end(),
		[&](const vk::ExtensionProperties& extension) { return strcmp(extension.extensionName, extensionName) == 0; });

	return foundExtension != m_vDeviceExtensions.end();
}

void CAPICompatibility::MemoryBarrierCompat(vk::CommandBuffer& commandBuffer, const vk::MemoryBarrier2KHR& barrier2KHR)
{
	if (m_vkVersion >= VK_API_VERSION_1_1)
	{
		vk::DependencyInfoKHR dependencyInfo{};
		dependencyInfo.memoryBarrierCount = 1;
		dependencyInfo.pMemoryBarriers = &barrier2KHR;

		commandBuffer.pipelineBarrier2KHR(&dependencyInfo);
	}
	else
	{
		vk::PipelineStageFlagBits srcStageMask;
		vk::PipelineStageFlagBits dstStageMask;
		auto barrier = convertMemoryBarrier2toMemoryBarrier(barrier2KHR, srcStageMask, dstStageMask);

		commandBuffer.pipelineBarrier(
			srcStageMask,
			dstStageMask, 
			vk::DependencyFlags(),
			1,
			&barrier,
			0,
			nullptr,
			0, 
			nullptr
		);
	}
}

void CAPICompatibility::ImageMemoryBarrierCompat(vk::CommandBuffer& commandBuffer, const vk::ImageMemoryBarrier2KHR& imageBarrier2KHR)
{
	if (m_vkVersion >= VK_API_VERSION_1_1)
	{
		vk::DependencyInfoKHR dependencyInfo{};
		dependencyInfo.imageMemoryBarrierCount = 1;
		dependencyInfo.pImageMemoryBarriers = &imageBarrier2KHR;

		commandBuffer.pipelineBarrier2KHR(dependencyInfo);
	}
	else
	{
		vk::PipelineStageFlagBits srcStageMask;
		vk::PipelineStageFlagBits dstStageMask;
		auto imageBarrier = convertImageMemoryBarrier2toImageMemoryBarrier(imageBarrier2KHR, srcStageMask, dstStageMask);

		commandBuffer.pipelineBarrier(
			srcStageMask,
			dstStageMask,
			vk::DependencyFlags(), 
			0, 
			nullptr, 
			0, 
			nullptr, 
			1, 
			&imageBarrier
		);
	}
}

void CAPICompatibility::TransitionImageLayoutTransfer(vk::CommandBuffer& commandBuffer, vk::ImageMemoryBarrier2& barrier)
{
	if (!setTransferImageLayoutFlags(barrier))
		log_error("Unsupported layout transition!");

	ImageMemoryBarrierCompat(commandBuffer, barrier);
}

void CAPICompatibility::TransitionImageLayoutGraphics(vk::CommandBuffer& commandBuffer, vk::ImageMemoryBarrier2& barrier)
{
	if (!setGraphicsImageLayoutFlags(barrier))
		log_error("Unsupported layout transition!");

	ImageMemoryBarrierCompat(commandBuffer, barrier);
}

vk::MemoryBarrier CAPICompatibility::convertMemoryBarrier2toMemoryBarrier(const vk::MemoryBarrier2KHR& barrier2KHR, vk::PipelineStageFlagBits& srcStageMask, vk::PipelineStageFlagBits& dstStageMask)
{
	vk::MemoryBarrier barrier{};
	barrier.srcAccessMask = static_cast<vk::AccessFlagBits>(static_cast<uint64_t>(barrier2KHR.srcAccessMask));
	barrier.dstAccessMask = static_cast<vk::AccessFlagBits>(static_cast<uint64_t>(barrier2KHR.dstAccessMask));

	srcStageMask = static_cast<vk::PipelineStageFlagBits>(static_cast<uint64_t>(barrier2KHR.srcStageMask));
	dstStageMask = static_cast<vk::PipelineStageFlagBits>(static_cast<uint64_t>(barrier2KHR.dstStageMask));

	return barrier;
}

vk::ImageMemoryBarrier CAPICompatibility::convertImageMemoryBarrier2toImageMemoryBarrier(const vk::ImageMemoryBarrier2KHR& imageBarrier2KHR, vk::PipelineStageFlagBits& srcStageMask, vk::PipelineStageFlagBits& dstStageMask)
{
	vk::ImageMemoryBarrier barrier{};

	barrier.srcAccessMask = static_cast<vk::AccessFlagBits>(static_cast<uint64_t>(imageBarrier2KHR.srcAccessMask));
	barrier.dstAccessMask = static_cast<vk::AccessFlagBits>(static_cast<uint64_t>(imageBarrier2KHR.dstAccessMask));
	barrier.oldLayout = imageBarrier2KHR.oldLayout;
	barrier.newLayout = imageBarrier2KHR.newLayout;
	barrier.srcQueueFamilyIndex = imageBarrier2KHR.srcQueueFamilyIndex;
	barrier.dstQueueFamilyIndex = imageBarrier2KHR.dstQueueFamilyIndex;
	barrier.image = imageBarrier2KHR.image;
	barrier.subresourceRange = imageBarrier2KHR.subresourceRange;

	srcStageMask = static_cast<vk::PipelineStageFlagBits>(static_cast<uint64_t>(imageBarrier2KHR.srcStageMask));
	dstStageMask = static_cast<vk::PipelineStageFlagBits>(static_cast<uint64_t>(imageBarrier2KHR.dstStageMask));

	return barrier;
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