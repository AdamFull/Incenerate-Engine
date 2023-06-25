#pragma once

#include "BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CLightsSystem : public ISystem
		{
		public:
			CLightsSystem() { name = "Lights system"; }
			virtual ~CLightsSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}