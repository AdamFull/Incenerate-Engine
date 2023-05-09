#pragma once

#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace graphics
	{
		class CAPICompatibility
		{
		public:
			CAPICompatibility(uint32_t vkVersion);

			void applyPhysicalDevice(const vk::PhysicalDevice& physicalDevice);
			bool checkDeviceExtensionSupport(const char* extensionName);

			void MemoryBarrierCompat(vk::CommandBuffer& commandBuffer, const vk::MemoryBarrier2KHR& barrier2KHR);
			void ImageMemoryBarrierCompat(vk::CommandBuffer& commandBuffer, const vk::ImageMemoryBarrier2KHR& imageBarrier2KHR);

			void TransitionImageLayoutTransfer(vk::CommandBuffer& commandBuffer, vk::ImageMemoryBarrier2& barrier);
			void TransitionImageLayoutGraphics(vk::CommandBuffer& commandBuffer, vk::ImageMemoryBarrier2& barrier);
		private:
			static vk::MemoryBarrier convertMemoryBarrier2toMemoryBarrier(const vk::MemoryBarrier2KHR& barrier2KHR, vk::PipelineStageFlagBits& srcStageMask, vk::PipelineStageFlagBits& dstStageMask);
			static vk::ImageMemoryBarrier convertImageMemoryBarrier2toImageMemoryBarrier(const vk::ImageMemoryBarrier2KHR& imageBarrier2KHR, vk::PipelineStageFlagBits& srcStageMask, vk::PipelineStageFlagBits& dstStageMask);
			static bool setTransferImageLayoutFlags(vk::ImageMemoryBarrier2KHR& imageBarrier2KHR);
			static bool setGraphicsImageLayoutFlags(vk::ImageMemoryBarrier2KHR& imageBarrier2KHR);
		private:
			uint32_t m_vkVersion{ 0u };
			std::vector<vk::ExtensionProperties> m_vDeviceExtensions;
		};
	}
}