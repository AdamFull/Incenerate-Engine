#pragma once

#include "ecs/systems/BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class C2DRenderSystem : public ISystem
		{
		public:
			virtual ~C2DRenderSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}