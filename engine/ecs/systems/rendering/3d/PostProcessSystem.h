#pragma once

#include "ecs/systems/rendering/BaseGraphicsSystem.h"

namespace engine
{
	namespace ecs
	{
		class CPostProcessSystem : public CBaseGraphicsSystem
		{
		public:
			CPostProcessSystem() { this->name = "Rendering post process system"; }
			virtual ~CPostProcessSystem() override;

			void __create() override;
			void __update(float fDt) override;
		private:
			size_t shader_fxaa{ invalid_index };
			size_t shader_brightdetect{ invalid_index };
			size_t shader_downsample{ invalid_index };
			size_t shader_blur{ invalid_index };
			size_t shader_tonemap{ invalid_index };

			size_t final_image{ invalid_index };
			size_t temp_image{ invalid_index };
			size_t temp_image_2{ invalid_index };
		};
	}
}