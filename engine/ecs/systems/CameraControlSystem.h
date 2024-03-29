#pragma once

#include "BaseSystem.h"
#include <interface/event/EventInterface.h>

#include "ecs/components/fwd.h"

namespace engine
{
	namespace ecs
	{
		class CCameraControlSystem : public ISystem
		{
		public:
			CCameraControlSystem() { name = "Camera system"; }
			virtual ~CCameraControlSystem() override = default;

			void __create() override;
			void __update(float fDt) override;

		private:
			static void updateCamera(FCameraComponent& camera, FTransformComponent& transform);
			static void recalculateProjection(FCameraComponent& camera, float xmax, float ymax);
			static void recalculateView(FCameraComponent& camera, FTransformComponent& transform);

			void rotate(FCameraComponent& camera, FTransformComponent& transform);
			void forward(FCameraComponent& camera, FTransformComponent& transform, float dir);
			void right(FCameraComponent& camera, FTransformComponent& transform, float dir);
			void up(FCameraComponent& camera, FTransformComponent& transform, float dir);

			void onKeyInput(const std::unique_ptr<IEvent>& event);
			void onMouseInput(const std::unique_ptr<IEvent>& event);

			void moveForward(bool bInv);
			void moveRight(bool bInv);
			void moveUp(bool bInv);
		private:
			glm::vec2 oldPos, cursorPos;
			float dt{ 0.f };
			bool bRotationPass{ false };
		};
	}
}