#include "CameraControlSystem.h"

#include "Engine.h"
#include "system/window/WindowHandle.h"

using namespace engine::ecs;
using namespace engine::system::window;

template<class _Ty>
_Ty rangeToRange(_Ty input, _Ty in_min, _Ty in_max, _Ty out_min, _Ty out_max)
{
	return (input - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void CCameraControlSystem::__create()
{
	EGCoordinator->addEventListener(Events::Input::Key, this, &CCameraControlSystem::onKeyInput);
	EGCoordinator->addEventListener(Events::Input::Mouse, this, &CCameraControlSystem::onMouseInput);

	for (auto& entity : mEntities)
	{
		auto& camera = EGCoordinator->getComponent<FCameraComponent>(entity);

		if (camera.active)
		{
			auto& transform = EGCoordinator->getComponent<FTransformComponent>(entity);
			//transform.rotation = { 0.514259, -0.318132, 0.796448 };
			camera.angleV = glm::degrees(-transform.rotation.y);
			camera.angleH = glm::degrees(glm::atan(transform.rotation.z / transform.rotation.x));
			EGCoordinator->setActiveCamera(entity);
			return;
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
	if (!bRotationPass)
		return;

	auto activeCamera = EGCoordinator->getActiveCamera();

	if (activeCamera)
	{
		auto& camera = EGCoordinator->getComponent<FCameraComponent>(activeCamera.value());
		auto& transform = EGCoordinator->getComponent<FTransformComponent>(activeCamera.value());

		auto fx = event.getParam<float>(Events::Input::MouseX);
		auto fy = event.getParam<float>(Events::Input::MouseY);

		float xmax = static_cast<float>(CWindowHandle::iWidth);
		float ymax = static_cast<float>(CWindowHandle::iHeight);

		oldPos = cursorPos;
		cursorPos = {
			rangeToRange(fx, 0.f, xmax, -1.f, 1.f),
			rangeToRange(fy, 0.f, ymax, -1.f, 1.f)
		};

		auto fdx = (cursorPos - oldPos) * dt;

		{
			float rotX = fdx.x / dt;
			float rotY = fdx.y / dt;

			camera.angleH += rotX * camera.sensitivity;
			if (camera.angleV + rotY * camera.sensitivity > 89)
				camera.angleV = 89;
			else if (camera.angleV + rotY * camera.sensitivity < -89)
				camera.angleV = -89;
			else
				camera.angleV += rotY * camera.sensitivity;

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

	bRotationPass = false;
}

void CCameraControlSystem::moveForward(bool bInv)
{
	float dir = bInv ? -1.f : 1.f;
	auto activeCamera = EGCoordinator->getActiveCamera();

	if (activeCamera)
	{
		auto& camera = EGCoordinator->getComponent<FCameraComponent>(activeCamera.value());
		auto& transform = EGCoordinator->getComponent<FTransformComponent>(activeCamera.value());

		transform.position += getForwardVector(transform) * dir * dt * camera.sensitivity;
		camera.moved = true;
	}
}

void CCameraControlSystem::moveRight(bool bInv)
{
	float dir = bInv ? -1.f : 1.f;
	auto activeCamera = EGCoordinator->getActiveCamera();

	if (activeCamera)
	{
		auto& camera = EGCoordinator->getComponent<FCameraComponent>(activeCamera.value());
		auto& transform = EGCoordinator->getComponent<FTransformComponent>(activeCamera.value());

		transform.position += getRightVector(transform) * dir * dt * camera.sensitivity;
		camera.moved = true;
	}
}

void CCameraControlSystem::moveUp(bool bInv)
{
	float dir = bInv ? -1.f : 1.f;
	auto activeCamera = EGCoordinator->getActiveCamera();

	if (activeCamera)
	{
		auto& camera = EGCoordinator->getComponent<FCameraComponent>(activeCamera.value());
		auto& transform = EGCoordinator->getComponent<FTransformComponent>(activeCamera.value());

		transform.position += getUpVector(transform) * dir * dt * camera.sensitivity;
		camera.moved = true;
	}
}