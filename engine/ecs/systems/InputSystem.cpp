#include "InputSystem.h"

#include "Engine.h"

using namespace engine::ecs;
using namespace engine::system::window;

void CInputSystem::__create()
{
	EGCoordinator->addEventListener(Events::Input::Key, this, &CInputSystem::onKeyInput);
	EGCoordinator->addEventListener(Events::Input::Mouse, this, &CInputSystem::onMouseInput);
	EGCoordinator->addEventListener(Events::Input::Axis, this, &CInputSystem::onAxisInput);
}

void CInputSystem::__update(float fDt)
{
	//log_debug("Key D pressed is {}", mKeys.test(EKeyCode::eKeyD));
}

void CInputSystem::onKeyInput(CEvent& event)
{
	mKeys = event.getParam<CKeyDecoder>(Events::Input::Key);
}

void CInputSystem::onMouseInput(CEvent& event)
{

}

void CInputSystem::onAxisInput(CEvent& event)
{

}