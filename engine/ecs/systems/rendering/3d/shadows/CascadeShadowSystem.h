#pragma once

#include "ecs/systems/rendering/BaseGraphicsSystem.h"

namespace engine
{
	namespace ecs
	{
		class CCascadeShadowSystem : public CBaseGraphicsSystem
		{
		public:
			CCascadeShadowSystem() { name = "Rendering cascade shadow system"; }
			virtual ~CCascadeShadowSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		private:
			size_t shader_id{ invalid_index };
		};
	}
}