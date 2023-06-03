#pragma once

#include "ecs/systems/rendering/BaseGraphicsSystem.h"

namespace engine
{
	namespace ecs
	{
		class CAmbientOcclusionSystem : public CBaseGraphicsSystem
		{
		public:
			CAmbientOcclusionSystem() { name = "Rendering ssao system"; }
			virtual ~CAmbientOcclusionSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		private:
			void updateOutputImage(const std::string& image_name, size_t& image_id);
			std::array<glm::vec4, SSAO_KERNEL_SIZE> vSSAOKernel;

			size_t noise_image{ invalid_index };
			size_t ssao_shader_id{ invalid_index };
			size_t gaussian_blur_shader_id{ invalid_index };
			size_t final_ao_image{ invalid_index };
			size_t stageId{ invalid_index };
		};
	}
}