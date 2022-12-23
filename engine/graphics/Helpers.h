#pragma once

#include "EngineStructures.h"
#include <vma/vk_mem_alloc.h>

namespace engine
{
	namespace graphics
	{
		struct FQueueFamilyIndices
		{
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;
			std::optional<uint32_t> computeFamily;
			std::optional<uint32_t> transferFamily;

			bool isComplete()
			{
				return graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value() && transferFamily.has_value();
			}
		};

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
			static std::vector<const char*> getRequiredExtensions(bool validation = false);

			static bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const std::vector<const char*>& deviceExtensions);

			static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>&);
			static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>);

			static bool hasStencilComponent(vk::Format format);
		};
	}
}