#pragma once

#include "ecs/systems/BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class C2DRenderSystem : public ISystem
		{
		public:
			C2DRenderSystem() { name = "2D render system"; }
			virtual ~C2DRenderSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}