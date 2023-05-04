#include "CameraControlSystem.h"

#include "Engine.h"

#include "ecs/components/components.h"

#include "ecs/helper.hpp"

#include <Helpers.h>

using namespace engine::ecs;
using namespace engine::graphics;

// TODO: it's temporary implementation. Maybe need to create special camera controller?

void CCameraControlSystem::__create()
{
	EGEngine->addEventListener(Events::Input::Key, this, &CCameraControlSystem::onKeyInput);
	EGEngine->addEventListener(Events::Input::Mouse, this, &CCameraControlSystem::onMouseInput);
}

void CCameraControlSystem::__update(float fDt)
{
	auto state = EGEngine->getState();

	auto view = registry->view<FTransformComponent, FCameraComponent>();
	for (auto [entity, transform, camera] : view.each())
	{
		if (camera.active)
			updateCamera(camera, transform);
	}

	dt = fDt;
}

void CCameraControlSystem::updateCamera(FCameraComponent& camera, FTransformComponent& transform)
{
	auto& graphics = EGEngine->getGraphics();
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);

	camera.forward = glm::normalize(transform.rotation);
	camera.right = glm::normalize(glm::cross(camera.forward, glm::vec3{ 0.0, 1.0, 0.0 }));
	camera.up = glm::normalize(glm::cross(camera.right, camera.forward));

	if (extent.width != camera.viewportDim.x || extent.height != camera.viewportDim.y)
	{
		camera.viewportDim.x = extent.width;
		camera.viewportDim.y = extent.height;

		recalculateProjection(camera, extent.width, extent.height);
	}

	if (camera.moved)
	{
		recalculateView(camera, transform);
		camera.moved = false;
	}

	camera.frustum.update(camera.view, camera.projection);
	camera.viewPos = transform.rposition;
}

void CCameraControlSystem::recalculateProjection(FCameraComponent& camera, float xmax, float ymax)
{
	if (camera.type == ECameraType::eOrthographic)
		//camera.projection = glm::ortho(camera.xmag, camera.ymag, xmax, ymax, camera.nearPlane, camera.farPlane);
		camera.projection = glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
	else
		camera.projection = glm::perspective(glm::radians(camera.fieldOfView), xmax / ymax, camera.nearPlane, camera.farPlane);

	camera.invProjection = glm::inverse(camera.projection);
}

void CCameraControlSystem::recalculateView(FCameraComponent& camera, FTransformComponent& transform)
{
	camera.view = glm::lookAt(transform.position, transform.position + camera.forward, camera.up);
	camera.invView = glm::inverse(camera.view);
}

void CCameraControlSystem::rotate(FCameraComponent& camera, FTransformComponent& transform)
{
	glm::vec2 delta = (cursorPos - oldPos) * dt;
	oldPos = cursorPos;

	if (delta.x != 0.0f || delta.y != 0.0f)
	{
		delta *= camera.sensitivity * camera.sensitivity;

		glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-delta.y, camera.right), glm::angleAxis(-delta.x, camera.up)));
		transform.rotation = glm::rotate(q, camera.forward);

		camera.moved = true;
	}
}

void CCameraControlSystem::forward(FCameraComponent& camera, FTransformComponent& transform, float dir)
{
	transform.position += camera.forward * dir * dt * camera.sensitivity;
	camera.moved = true;
}

void CCameraControlSystem::right(FCameraComponent& camera, FTransformComponent& transform, float dir)
{
	transform.position += camera.right * dir * dt * camera.sensitivity;
	camera.moved = true;
}

void CCameraControlSystem::up(FCameraComponent& camera, FTransformComponent& transform, float dir)
{
	transform.position += camera.up * dir * dt * camera.sensitivity;
	camera.moved = true;
}

void CCameraControlSystem::onKeyInput(const std::unique_ptr<IEvent>& event)
{
	auto keys = event->getParam<CKeyDecoder>(Events::Input::Key);

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
	if (keys.test(EKeyCode::eKeyLCtrl))
		moveUp(true);
	if (keys.test(EKeyCode::eMouseMiddle))
		bRotationPass = true;
}

void CCameraControlSystem::onMouseInput(const std::unique_ptr<IEvent>& event)
{
	auto& window = EGEngine->getWindow();

	int32_t wwidth{ 0 }, wheight{ 0 };
	window->getWindowSize(&wwidth, &wheight);

	auto fx = event->getParam<float>(Events::Input::MouseX);
	auto fy = event->getParam<float>(Events::Input::MouseY);

	float xmax = static_cast<float>(wwidth);
	float ymax = static_cast<float>(wheight);

	oldPos = cursorPos;
	cursorPos = {
		rangeToRange(fx, 0.f, xmax, -1.f, 1.f),
		rangeToRange(fy, 0.f, ymax, -1.f, 1.f)
	};

	if (!bRotationPass)
		return;


	auto view = registry->view<FTransformComponent, FCameraComponent>();
	for (auto [entity, transform, camera] : view.each())
	{
		if (camera.active && camera.controllable)
			rotate(camera, transform);
	}

	bRotationPass = false;
}

void CCameraControlSystem::moveForward(bool bInv)
{
	float dir = bInv ? -1.f : 1.f;

	auto view = registry->view<FTransformComponent, FCameraComponent>();
	for (auto [entity, transform, camera] : view.each())
	{
		if (camera.active && camera.controllable)
			forward(camera, transform, dir);
	}
}

void CCameraControlSystem::moveRight(bool bInv)
{
	float dir = bInv ? -1.f : 1.f;

	auto view = registry->view<FTransformComponent, FCameraComponent>();
	for (auto [entity, transform, camera] : view.each())
	{
		if (camera.active && camera.controllable)
			right(camera, transform, dir);
	}
}

void CCameraControlSystem::moveUp(bool bInv)
{
	float dir = bInv ? -1.f : 1.f;

	auto view = registry->view<FTransformComponent, FCameraComponent>();
	for (auto [entity, transform, camera] : view.each())
	{
		if (camera.active && camera.controllable)
			up(camera, transform, dir);
	}
}