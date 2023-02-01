#pragma once

#include "BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CPhysicsSystem : public ISystem
		{
		public:
			CPhysicsSystem() { name = "Physics system"; }
			virtual ~CPhysicsSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}