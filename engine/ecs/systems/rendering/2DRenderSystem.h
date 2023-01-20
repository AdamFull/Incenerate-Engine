#pragma once

#include "BaseGraphicsSystem.h"

namespace engine
{
	namespace ecs
	{
		class C2DRenderSystem : public CBaseGraphicsSystem
		{
		public:
			C2DRenderSystem() { name = "2D render system"; }
			virtual ~C2DRenderSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}