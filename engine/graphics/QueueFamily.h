#pragma once

#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace graphics
	{
		namespace family
		{
			using namespace utl;

			constexpr inline uint32_t graphics = "graphics"_utl_hash;
			constexpr inline uint32_t graphics_compute = "graphics_compute"_utl_hash;
			constexpr inline uint32_t compute = "compute"_utl_hash;
			constexpr inline uint32_t graphics_transfer = "graphics_transfer"_utl_hash;
			constexpr inline uint32_t transfer = "transfer"_utl_hash;
			constexpr inline uint32_t present = "present"_utl_hash;
		}

		class CQueueFamilies
		{
		public:
			void initialize(const vk::PhysicalDevice& vkPhysical, vk::SurfaceKHR& vkSurface);

			std::vector<vk::DeviceQueueCreateInfo> getCreateInfos();
			void clearCreateInfo();

			vk::Queue createQueue(uint32_t type, vk::Device& vkDevice);
			const uint32_t getFamilyIndex(uint32_t type) const;
			const std::vector<uint32_t>& getUniqueFamilies() const;
			bool isValid();

		private:
			bool isFamilySelected(uint32_t type);
		private:
			std::vector<uint32_t> vUniqueFamilies;
			std::map<uint32_t, uint32_t> mFamilyUsages;
			std::map<uint32_t, uint32_t> mSelectedFamilies;
			std::map<uint32_t, vk::QueueFamilyProperties> mQueueFamilies;
			std::map<uint32_t, std::vector<float>> mPriorities;
		};
	}
}