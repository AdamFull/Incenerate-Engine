#pragma once

#include "ecs/core/System.hpp"

namespace engine
{
	namespace ecs
	{
		class C3DRenderSystem : public ISystem
		{
		public:
			virtual ~C3DRenderSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}