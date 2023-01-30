#pragma once

#include "postprocess/FXAAEffect.h"
#include "postprocess/DOFEffect.h"
#include "postprocess/BloomEffect.h"
#include "postprocess/ChromaticAberrationEffect.h"
#include "postprocess/VignetteEffect.h"
#include "postprocess/TonemapEffect.h"

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
			CDOFEffect dof;
			CBloomEffect bloom;
			CChromaticAberrationEffect chromatic_aberration;
			CVignetteEffect vignette;
			CTonemapEffect tonemap;

			size_t final_image{ invalid_index };
			size_t shader_id{ invalid_index };
		};
	}
}