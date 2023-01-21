#pragma once

#include "postprocess/BloomEffect.h"
#include "postprocess/FXAAEffect.h"
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
			CFXAAEffect fxaa;
			CBloomEffect bloom;

			size_t shader_tonemap{ invalid_index };

			size_t final_image{ invalid_index };
		};
	}
}