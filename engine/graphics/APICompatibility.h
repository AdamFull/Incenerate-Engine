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

			static void printDeviceExtensions(const vk::PhysicalDevice& physicalDevice);

			static bool check(vk::Result result);
			static std::string get_error(vk::Result);

			static bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);

			static bool checkDeviceExtensionSupport(const vk::PhysicalDevice& physicalDevice, const char* extensionName);
			static bool checkDeviceExtensionEnabled(const vk::PhysicalDevice& physicalDevice, const char* extensionName);

			static bool checkInstanceExtensionSupport(const char* extensionName);
			static bool checkInstanceExtensionEnabled(const char* extensionName);

			static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>&);
			static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>);

			static const std::vector<const char*>& getRequiredDeviceExtensions();
			static const std::vector<const char*>& getOptionalDeviceExtensions();
			static std::vector<const char*> getDeviceAvaliableExtensions(const vk::PhysicalDevice& physicalDevice);

			static const std::vector<const char*>& getRequiredInstanceExtensions();
			static const std::vector<const char*>& getOptionalInstanceExtensions();
			static std::vector<const char*> getInstanceAvaliableExtensions();

			static void memoryBarrierCompat(vk::CommandBuffer& commandBuffer, const vk::MemoryBarrier2KHR& barrier2KHR);
			static void imageMemoryBarrierCompat(vk::CommandBuffer& commandBuffer, const vk::ImageMemoryBarrier2KHR& imageBarrier2KHR);

			static void transitionImageLayoutTransfer(vk::CommandBuffer& commandBuffer, vk::ImageMemoryBarrier2& barrier);
			static void transitionImageLayoutGraphics(vk::CommandBuffer& commandBuffer, vk::ImageMemoryBarrier2& barrier);

			static void applyDescriptorIndexingFeatures(vk::PhysicalDeviceVulkan12Features& features);
			static void applyDescriptorIndexingFeatures(vk::PhysicalDeviceDescriptorIndexingFeatures& features);

			static void applyTimelineSemaphoreFeatures(vk::PhysicalDeviceVulkan12Features& features);
			static void applyTimelineSemaphoreFeatures(vk::PhysicalDeviceTimelineSemaphoreFeaturesKHR& features);

			static bool bindlessSupport;
			static bool shaderObjectSupport;
			static bool timelineSemaphoreSupport;
		private:
			static bool setTransferImageLayoutFlags(vk::ImageMemoryBarrier2KHR& imageBarrier2KHR);
			static bool setGraphicsImageLayoutFlags(vk::ImageMemoryBarrier2KHR& imageBarrier2KHR);
		};
	}
}