#pragma once

#include "graphics/Frustum.h"
#include "GameObject.h"

namespace engine
{
	namespace game
	{
		class CCamera : public CGameObject
		{
		public:
			void create() override;
			void update(float fDT) override;

			void onResize(uint32_t width, uint32_t height);

			void moveForward(bool bInv);
			void moveRight(bool bInv);
			void moveUp(bool bInv);

			void lookAt(float dX, float dY);

			const glm::mat4& getProjection(bool flipY = false) const;
			const glm::mat4& getView(bool flipY = false) const;
			glm::vec3& getViewPos();

			glm::vec3 getForwardVector() const;
			glm::vec3 getUpVector() const;
			glm::vec3 getRightVector() const;

			const float getFieldOfView() const { return fieldOfView; }
			void setFieldOfView(float value) { fieldOfView = value; }

			const float getNearPlane() const { return nearPlane; }
			void setNearPlane(float value) { nearPlane = value; }

			const float getFarPlane() const { return farPlane; }
			void setFarPlane(float value) { farPlane = value; }

			const float getAspect() { return aspect; }
			void setAspect(float value) { aspect = value; }

			const FFrustum& getFrustum() { return frustum; }

		private:
			void recalculateView();
			void recalculateProjection();
		private:
			FFrustum frustum;
			float dt{ 0.0 }, aspect{ 1.7f }, fieldOfView{ 45.f }, nearPlane{ 0.1f }, farPlane{ 128.f }, sensitivity{ 15.f };
			float angleH{ 0.f }, angleV{ 0.f };

			uint32_t width{ 0 }, height{ 0 };
			bool bMoved{ true };
			glm::mat4 view, invView, projection, invProjection;
		};
	}
}