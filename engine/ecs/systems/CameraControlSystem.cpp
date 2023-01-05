#include "CameraControlSystem.h"

#include "Engine.h"
#include "system/window/WindowHandle.h"

using namespace engine::ecs;
using namespace engine::system::window;

void CCameraControlSystem::__create()
{
	for (auto& entity : mEntities)
	{
		auto& camera = EGCoordinator->getComponent<FCameraComponent>(entity);

		if (camera.active)
		{
			auto& transform = EGCoordinator->getComponent<FTransformComponent>(entity);
			transform.position = { -6.3434, 15.9819, -22.8122 };
			transform.rotation = { 0.514259, -0.318132, 0.796448 };
			camera.angleV = glm::degrees(-transform.rotation.y);
			camera.angleH = glm::degrees(glm::atan(transform.rotation.z / transform.rotation.x));
		}
	}
}

void CCameraControlSystem::__update(float fDt)
{
	dt = fDt;

	for (auto& entity : mEntities)
	{
		auto& camera = EGCoordinator->getComponent<FCameraComponent>(entity);

		if (camera.active)
		{
			auto& transform = EGCoordinator->getComponent<FTransformComponent>(entity);

			auto width = CWindowHandle::iWidth;
			auto height = CWindowHandle::iHeight;

			if (width != camera.viewportDim.x && height != camera.viewportDim.y)
			{
				camera.viewportDim.x = width;
				camera.viewportDim.y = height;

				recalculateProjection(camera);
			}

			if (camera.moved)
			{
				recalculateView(camera, transform);
				camera.moved = false;
			}

			// TODO: bind camera to render system

			camera.frustum.update(camera.view, camera.projection);
		}
	}
}

void CCameraControlSystem::recalculateProjection(FCameraComponent& camera)
{
	camera.projection = glm::perspective(glm::radians(camera.fieldOfView), camera.aspect, camera.nearPlane, camera.farPlane);
	camera.invProjection = glm::inverse(camera.projection);
}

void CCameraControlSystem::recalculateView(FCameraComponent& camera, FTransformComponent& transform)
{
	camera.view = glm::lookAt(transform.position, transform.position + getForwardVector(transform), getUpVector(transform));
	camera.invView = glm::inverse(camera.view);
}

glm::vec3 CCameraControlSystem::getForwardVector(FTransformComponent& transform) const
{
	return glm::normalize(transform.rotation);
}

glm::vec3 CCameraControlSystem::getUpVector(FTransformComponent& transform) const
{
	return glm::normalize(glm::cross(getRightVector(transform), getForwardVector(transform)));
}

glm::vec3 CCameraControlSystem::getRightVector(FTransformComponent& transform) const
{
	return glm::normalize(glm::cross(getForwardVector(transform), glm::vec3{ 0.0, 1.0, 0.0 }));
}