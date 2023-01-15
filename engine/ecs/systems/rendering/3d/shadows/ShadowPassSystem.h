#pragma once

#include "DirectionalShadowSystem.h"
#include "OmniShadowSystem.h"

namespace engine
{
	namespace ecs
	{
		class CShadowPassSystem : public ISystem
		{
		public:
			CShadowPassSystem() { name = "Rendering shadow system"; }
			virtual ~CShadowPassSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}