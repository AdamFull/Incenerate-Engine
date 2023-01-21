#pragma once

#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace ecs
	{
		struct FBloomMip
		{
			glm::vec2 size;
			vk::ImageView view{ VK_NULL_HANDLE };
		};

		class CBloomEffect
		{
		public:
			void create();
			void update();
			size_t render(bool enable, size_t source);
		private:
			void init();
			std::vector<FBloomMip> vMips;

			size_t shader_brightdetect{ invalid_index };
			size_t shader_downsample{ invalid_index };
			size_t shader_upsample{ invalid_index };
			size_t shader_applybloom{ invalid_index };

			size_t bloom_image{ invalid_index };
			size_t final_image{ invalid_index };
		};
	}
}