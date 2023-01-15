#pragma once

#include "BaseSystem.h"

#include "ecs/components/fwd.h"

namespace engine
{
	namespace ecs
	{
		class CAudioSystem : public ISystem
		{
		public:
			CAudioSystem() { name = "Audio system"; }
			virtual ~CAudioSystem() override;

			void __create() override;
			void __update(float fDt) override;
		private:
			void updateListener(FCameraComponent& camera, FTransformComponent& transform);
		};
	}
}