#pragma once

#include "ecs/systems/rendering/BaseGraphicsSystem.h"

namespace engine
{
	namespace ecs
	{
		class CGlobalIlluminationSystem : public CBaseGraphicsSystem
		{
		public:
			CGlobalIlluminationSystem() { name = "Rendering ssdo system"; }
			virtual ~CGlobalIlluminationSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		private:
			std::array<glm::vec4, SSAO_KERNEL_SIZE> vSSDOKernel;

			size_t noise_image{ invalid_index };
			size_t shader_id{ invalid_index };
		};
	}
}