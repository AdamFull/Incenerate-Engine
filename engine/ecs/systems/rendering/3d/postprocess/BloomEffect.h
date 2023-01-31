#pragma once

#include <vulkan/vulkan.hpp>
#include "ecs/components/fwd.h"

namespace engine
{
	namespace ecs
	{
		class CBloomEffect
		{
		public:
			~CBloomEffect();

			void create();
			void update();
			size_t render(FCameraComponent* camera, size_t source);
		private:
			void init();
			std::vector<glm::vec2> vMips;
			const uint32_t mipLevels{ 5 };

			graphics::CAPIHandle* graphics{ nullptr };

			size_t shader_brightdetect{ invalid_index };
			size_t shader_downsample{ invalid_index };
			size_t shader_upsample{ invalid_index };
			size_t shader_applybloom{ invalid_index };

			size_t bloom_image{ invalid_index };
			size_t final_image{ invalid_index };
		};
	}
}