#pragma once

#include "EngineStructures.h"
#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace graphics
	{
		struct FSwapChainSupportDetails
		{
			vk::SurfaceCapabilitiesKHR capabilities;
			std::vector<vk::SurfaceFormatKHR> formats;
			std::vector<vk::PresentModeKHR> presentModes;
		};

		class VkHelper
		{
		public:

			static bool check(vk::Result result);
			static bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);

			static bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const std::vector<const char*>& deviceExtensions);

			static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>&);
			static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>);

			static bool hasStencilComponent(vk::Format format);

			static void BarrierFromComputeToCompute();
			static void BarrierFromComputeToCompute(vk::CommandBuffer& commandBuffer);
			static void BarrierFromComputeToGraphics();
			static void BarrierFromComputeToGraphics(vk::CommandBuffer& commandBuffer);
			static void BarrierFromGraphicsToCompute(size_t image_id = invalid_index);
			static void BarrierFromGraphicsToCompute(vk::CommandBuffer& commandBuffer, size_t image_id = invalid_index);
			static void BarrierFromGraphicsToTransfer(size_t image_id = invalid_index);
			static void BarrierFromGraphicsToTransfer(vk::CommandBuffer& commandBuffer, size_t image_id = invalid_index);
		};
	}
}