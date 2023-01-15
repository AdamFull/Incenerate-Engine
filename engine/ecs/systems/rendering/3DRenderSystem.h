#pragma once

#include "ecs/systems/BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class C3DRenderSystem : public ISystem
		{
		public:
			C3DRenderSystem() { name = "3D render system"; }
			virtual ~C3DRenderSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}