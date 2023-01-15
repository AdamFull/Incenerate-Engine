#pragma once

#include "ecs/systems/BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class COmniShadowSystem : public ISystem
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