#pragma once

#include "ecs/systems/rendering/BaseGraphicsSystem.h"

namespace engine
{
	namespace ecs
	{
		class CComputeSkySystem : public CBaseGraphicsSystem
		{
		public:
			CComputeSkySystem() { this->name = "Compute environment system"; }
			virtual ~CComputeSkySystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		private:
			void computeAtmosphericScattering(const glm::mat4& projection, const glm::mat4& view);
			void computeClouds(const glm::mat4& projection, const glm::mat4& view);
			void computeSun(const glm::mat4& projection, const glm::mat4& view);
			void computeComposition(const glm::mat4& projection, const glm::mat4& view);
		private:
			size_t atmosphere_shader_id{ invalid_index };
			size_t atmosphere_tex_id{ invalid_index };

			size_t clouds_shader_id{ invalid_index };
			size_t clouds_tex_id{ invalid_index };

			size_t sun_shader_id{ invalid_index };
			size_t sun_tex_id{ invalid_index };

			size_t composition_shader_id{ invalid_index };
			size_t composition_tex_id{ invalid_index };

			size_t cloudmap_tex_id{ invalid_index };
			size_t perlin_noise_tex_id{ invalid_index };
			size_t cellular_noise_tex_id{ invalid_index };
			size_t noise_tex_id{ invalid_index };

			float fTime{ 0.0f };
		};
	}
}