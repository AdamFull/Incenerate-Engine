#pragma once

#include "ecs/systems/rendering/BaseGraphicsSystem.h"

namespace engine
{
	namespace ecs
	{
		class COmniShadowSystem : public CBaseGraphicsSystem
		{
		public:
			COmniShadowSystem() { name = "Rendering omni shadow system"; }
			virtual ~COmniShadowSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		private:
			size_t shader_id{ invalid_index };
		};
	}
}