#pragma once

#include "BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CAnimationSystem : public ISystem
		{
		public:
			CAnimationSystem() { name = "Animation system"; }

			void __create() override;
			void __update(float fDt) override;
			
		};
	}
}