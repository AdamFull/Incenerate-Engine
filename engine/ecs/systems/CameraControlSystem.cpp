#include "CameraControlSystem.h"

#include "Engine.h"
#include "system/window/WindowHandle.h"

#include "ecs/helper.hpp"

using namespace engine::ecs;
using namespace engine::system::window;

template<class _Ty>
_Ty rangeToRange(_Ty input, _Ty in_min, _Ty in_max, _Ty out_min, _Ty out_max)
{
	return (input - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// TODO: it's temporary implementation. Maybe need to create special camera controller?

void CCameraControlSystem::__create()
{
	auto& registry = EGCoordinator;

	EGEngine->addEventListener(Events::Input::Key, this, &CCameraControlSystem::onKeyInput);
	EGEngine->addEventListener(Events::Input::Mouse, this, &CCameraControlSystem::onMouseInput);

	auto view = registry.view<FTransformComponent, FCameraComponent>();
	for (auto [entity, transform, camera] : view.each())
	{
		camera.active = true;
		camera.angleV = glm::degrees(-transform.rotation.y);
		camera.angleH = glm::degrees(glm::atan(transform.rotation.z / transform.rotation.x));
	}
}

void CCameraControlSystem::__update(float fDt)
{
	auto& device = EGGraphics->getDevice();
	auto& registry = EGCoordinator;

	auto view = registry.view<FTransformComponent, FCameraComponent>();
	for (auto [entity, transform, camera] : view.each())
	{
		if (camera.active)
		{
			auto extent = device->getExtent(true);
			camera.aspect = device->getAspect(true);

			camera.forward = glm::normalize(transform.rotation);
			camera.right = glm::normalize(glm::cross(camera.forward, glm::vec3{ 0.0, 1.0, 0.0 }));
			camera.up = glm::normalize(glm::cross(camera.right, camera.forward));

			if (extent.width != camera.viewportDim.x && extent.height != camera.viewportDim.y)
			{
				camera.viewportDim.x = extent.width;
				camera.viewportDim.y = extent.height;

				recalculateProjection(camera);
			}

			if (camera.moved)
			{
				recalculateView(camera, transform);
				camera.moved = false;
			}

			camera.frustum.update(camera.view, camera.projection);
		}
	}

	dt = fDt;
}

void CCameraControlSystem::recalculateProjection(FCameraComponent& camera)
{
	camera.projection = glm::perspective(glm::radians(camera.fieldOfView), camera.aspect, camera.nearPlane, camera.farPlane);
	camera.invProjection = glm::inverse(camera.projection);
}

void CCameraControlSystem::recalculateView(FCameraComponent& camera, FTransformComponent& transform)
{
	camera.view = glm::lookAt(transform.position, transform.position + camera.forward, camera.up);
	camera.invView = glm::inverse(camera.view);
}

void CCameraControlSystem::onKeyInput(CEvent& event)
{
	auto keys = event.getParam<CKeyDecoder>(Events::Input::Key);

	if (keys.test(EKeyCode::eKeyW))
		moveForward(false);
	if (keys.test(EKeyCode::eKeyS))
		moveForward(true);
	if (keys.test(EKeyCode::eKeyA))
		moveRight(true);
	if (keys.test(EKeyCode::eKeyD))
		moveRight(false);
	if (keys.test(EKeyCode::eKeySpace))
		moveUp(false);
	if (keys.test(EKeyCode::eKeyZ))
		moveUp(true);
	if (keys.test(EKeyCode::eMouseMiddle))
		bRotationPass = true;
}

void CCameraControlSystem::onMouseInput(CEvent& event)
{
	auto& registry = EGCoordinator;

	auto fx = event.getParam<float>(Events::Input::MouseX);
	auto fy = event.getParam<float>(Events::Input::MouseY);

	float xmax = static_cast<float>(CWindowHandle::iWidth);
	float ymax = static_cast<float>(CWindowHandle::iHeight);

	oldPos = cursorPos;
	cursorPos = {
		rangeToRange(fx, 0.f, xmax, -1.f, 1.f),
		rangeToRange(fy, 0.f, ymax, -1.f, 1.f)
	};

	if (!bRotationPass)
		return;

	auto view = registry.view<FTransformComponent, FCameraComponent>();
	for (auto [entity, transform, camera] : view.each())
	{
		if (camera.active)
		{
			auto fdx = (cursorPos - oldPos) * dt;

			{
				float rotX = (fdx.x / dt) * camera.sensitivity * 1.5f;
				float rotY = (fdx.y / dt) * camera.sensitivity * 1.5f;

				camera.angleH += rotX;
				if (camera.angleV + rotY > 89)
					camera.angleV = 89;
				else if (camera.angleV + rotY < -89)
					camera.angleV = -89;
				else
					camera.angleV += rotY;

				const float w{ cos(glm::radians(camera.angleV)) * -cos(glm::radians(camera.angleH)) };
				const float u{ -sin(glm::radians(camera.angleV)) };
				const float v{ cos(glm::radians(camera.angleV)) * -sin(glm::radians(camera.angleH)) };

				auto newrot = glm::normalize(glm::vec3(w * -1.f, u, v * -1.f));
				if (transform.rotation != newrot)
				{
					transform.rotation = newrot;
					camera.moved = true;
				}
			}
		}
	}

	bRotationPass = false;
}

void CCameraControlSystem::moveForward(bool bInv)
{
	auto& registry = EGCoordinator;
	float dir = bInv ? -1.f : 1.f;

	auto view = registry.view<FTransformComponent, FCameraComponent>();
	for (auto [entity, transform, camera] : view.each())
	{
		if (camera.active)
		{
			transform.position += camera.forward * dir * dt * camera.sensitivity;
			camera.moved = true;
		}
	}
}

void CCameraControlSystem::moveRight(bool bInv)
{
	auto& registry = EGCoordinator;
	float dir = bInv ? -1.f : 1.f;

	auto view = registry.view<FTransformComponent, FCameraComponent>();
	for (auto [entity, transform, camera] : view.each())
	{
		if (camera.active)
		{
			transform.position += camera.right * dir * dt * camera.sensitivity;
			camera.moved = true;
		}
	}
}

void CCameraControlSystem::moveUp(bool bInv)
{
	auto& registry = EGCoordinator;
	float dir = bInv ? -1.f : 1.f;

	auto view = registry.view<FTransformComponent, FCameraComponent>();
	for (auto [entity, transform, camera] : view.each())
	{
		if (camera.active)
		{
			transform.position += camera.up * dir * dt * camera.sensitivity;
			camera.moved = true;
		}
	}
}