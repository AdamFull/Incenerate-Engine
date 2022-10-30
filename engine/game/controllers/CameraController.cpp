#include "CameraController.h"

using namespace engine::game;
using namespace engine::system::input;

void CCameraController::create(CInputMapper* pInputMapper)
{

}

void CCameraController::update(float fDT)
{

}

void CCameraController::setCamera(std::shared_ptr<CCamera> camera)
{
	pCamera = camera;
}

const std::shared_ptr<CCamera> CCameraController::getCamera()
{
	return pCamera;
}