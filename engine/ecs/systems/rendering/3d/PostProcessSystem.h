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
			size_t shader_id_tonemap{ invalid_index };
			size_t shader_id_downsample{ invalid_index };
			size_t shader_id_blur{ invalid_index };
			size_t bloom_image{ invalid_index };
			size_t bloom_image2{ invalid_index };
		};
	}
}