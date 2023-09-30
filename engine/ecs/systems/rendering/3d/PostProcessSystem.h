#pragma once

#include "ecs/systems/rendering/BaseGraphicsSystem.h"

namespace engine
{
	namespace ecs
	{
		class CPostProcessSystem : public CBaseGraphicsSystem
		{
		public:
			CPostProcessSystem() { name = "Rendering post process system"; }
			virtual ~CPostProcessSystem() override;

			void __create() override;
			void __update(float fDt) override;
		private:
			void update_images();
			void render();
		private:
			std::vector<glm::vec3> vMips;
			const uint32_t bloomMipLevels{ 5 };

			size_t final_image{ invalid_index };
			size_t blurred_image{ invalid_index };
			size_t bloom_image{ invalid_index };
			size_t avg_lum_image{ invalid_index };
			size_t shader{ invalid_index };
			size_t shader_blur{ invalid_index };
			size_t shader_downsample{ invalid_index };
			size_t shader_upsample{ invalid_index };
			size_t shader_avg_brightness{ invalid_index };
		};
	}
}