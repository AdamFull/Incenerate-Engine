#pragma once

#include "BaseGraphicsSystem.h"

namespace engine
{
	namespace ecs
	{
		class C3DRenderSystem : public CBaseGraphicsSystem
		{
		public:
			C3DRenderSystem() { name = "3D render system"; }
			virtual ~C3DRenderSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}