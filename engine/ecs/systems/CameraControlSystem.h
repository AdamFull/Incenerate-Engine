#pragma once

#include "ecs/core/System.hpp"
#include "ecs/core/Event.hpp"

#include "ecs/components/CameraComponent.h"
#include "ecs/components/TransformComponent.h"

namespace engine
{
	namespace ecs
	{
		class CCameraControlSystem : public ISystem
		{
		public:
			virtual ~CCameraControlSystem() override = default;

			void __create() override;
			void __update(float fDt) override;

		private:
			void recalculateProjection(FCameraComponent& camera);
			void recalculateView(FCameraComponent& camera, FTransformComponent& transform);

			glm::vec3 getForwardVector(FTransformComponent& transform) const;
			glm::vec3 getUpVector(FTransformComponent& transform) const;
			glm::vec3 getRightVector(FTransformComponent& transform) const;

			void onKeyInput(CEvent& event);
			void onMouseInput(CEvent& event);

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