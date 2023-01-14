#pragma once

#include "BaseSystem.h"

#include "ecs/components/CameraComponent.h"
#include "ecs/components/TransformComponent.h"

namespace engine
{
	namespace ecs
	{
		class CAudioSystem : public ISystem
		{
		public:
			virtual ~CAudioSystem() override;

			void __create() override;
			void __update(float fDt) override;
		private:
			void updateListener(FCameraComponent& camera, FTransformComponent& transform);
		};
	}
}