#include "FreeCameraController.h"

#include "system/input/InputMapper.h"

#include "window/WindowHandle.h"

using namespace engine;
using namespace engine::system::input;

void CFreeCameraController::create(system::input::CInputMapper* pInputMapper)
{
	pCamera = std::make_unique<CCamera>();
	pCamera->create();

	pInputMapper->createAction("CameraMovement", EKeyCode::eKeyW, EKeyCode::eKeyS, EKeyCode::eKeyA,
		EKeyCode::eKeyD, EKeyCode::eKeySpace, EKeyCode::eKeyLCtrl, EKeyCode::eMouseMiddle);
	pInputMapper->createAction("CameraRotation", EKeyCode::eCursorDelta);

	pInputMapper->bindAction("CameraMovement", EKeyState::ePressed, this, &CFreeCameraController::cameraMovement);
	pInputMapper->bindAxis("CameraRotation", this, &CFreeCameraController::mouseRotation);
}

void CFreeCameraController::update(float fDT)
{
    pCamera->update(fDT);
}

const std::unique_ptr<CCamera>& CFreeCameraController::getCamera()
{
	return pCamera;
}

void CFreeCameraController::cameraMovement(system::input::EKeyCode eKey, system::input::EKeyState eState)
{
    switch (eKey)
    {
    case EKeyCode::eKeyW:
        pCamera->moveForward(false);
        break;
    case EKeyCode::eKeyS:
        pCamera->moveForward(true);
        break;
    case EKeyCode::eKeyA:
        pCamera->moveRight(true);
        break;
    case EKeyCode::eKeyD:
        pCamera->moveRight(false);
        break;
    case EKeyCode::eKeySpace:
        pCamera->moveUp(false);
        break;
    case EKeyCode::eKeyLCtrl:
        pCamera->moveUp(true);
        break;
    case EKeyCode::eMouseMiddle:
        m_bRotatePass = true;
        break;

    default:
        break;
    }
}

void CFreeCameraController::mouseRotation(float fX, float fY)
{
    if (!m_bRotatePass)
        return;

    pCamera->lookAt(fX, fY);
    m_bRotatePass = false;
}

void CFreeCameraController::cameraToPoint(float fX, float fY)
{

}