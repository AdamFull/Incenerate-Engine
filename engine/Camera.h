#pragma once

#include "Frustum.h"
#include "EngineStructures.h"

namespace engine
{
	class CCamera
	{
	public:
		void create();
		void update(float fDT);

		void moveForward(bool bInv);
		void moveRight(bool bInv);
		void moveUp(bool bInv);

		void lookAt(float dX, float dY);

		glm::mat4 getProjection(bool flipY = false) const;
		glm::mat4 getView(bool flipY = false) const;
		glm::vec4& getViewPos();

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

	private:
		glm::vec4 viewPos{};
		FFrustum frustum;
		FTransform transform;
		float dt{ 0.0 }, aspect{1.7f}, fieldOfView{45.f}, nearPlane{0.1f}, farPlane{60.f}, sensitivity{15.f};
		float angleH{ 0.f }, angleV{ 0.f };
	};
}