#pragma once

#include "EngineStructures.h"
#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace graphics
	{
		class APICompatibility
		{
		public:
			static uint32_t getVulkanVersion(ERenderApi eAPI);

			static bool check(vk::Result result);

			static bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);

			static bool checkExtensionSupport(const vk::PhysicalDevice& physicalDevice, const char* extensionName);

			static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>&);
			static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>);

			static const std::vector<const char*>& getRequiredDeviceExtensions();
			static const std::vector<const char*>& getOptionalDeviceExtensions();
			static std::vector<const char*> getDeviceAvaliableExtensions(const vk::PhysicalDevice& physicalDevice);

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