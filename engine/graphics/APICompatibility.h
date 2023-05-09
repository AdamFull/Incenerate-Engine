#pragma once

#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace graphics
	{
		class CAPICompatibility
		{
		public:
			static bool checkExtensionSupport(const vk::PhysicalDevice& physicalDevice, const char* extensionName);

			static void memoryBarrierCompat(vk::CommandBuffer& commandBuffer, const vk::MemoryBarrier2KHR& barrier2KHR);
			static void imageMemoryBarrierCompat(vk::CommandBuffer& commandBuffer, const vk::ImageMemoryBarrier2KHR& imageBarrier2KHR);

			static void transitionImageLayoutTransfer(vk::CommandBuffer& commandBuffer, vk::ImageMemoryBarrier2& barrier);
			static void transitionImageLayoutGraphics(vk::CommandBuffer& commandBuffer, vk::ImageMemoryBarrier2& barrier);
		private:
			static bool setTransferImageLayoutFlags(vk::ImageMemoryBarrier2KHR& imageBarrier2KHR);
			static bool setGraphicsImageLayoutFlags(vk::ImageMemoryBarrier2KHR& imageBarrier2KHR);
		};
	}
}