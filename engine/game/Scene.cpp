#include "Scene.h"

#include "Engine.h"
#include "controllers/FreeCameraController.h"

using namespace engine;
using namespace engine::game;

CScene::CScene()
{
	pCameraController = std::make_unique<CFreeCameraController>();
}

void CScene::create()
{
	CGameObject::create();

	pCameraController->create(CEngine::getInstance()->getInputMapper().get());
}

void CScene::update(float fDT)
{
	CGameObject::update(fDT);

	pCameraController->update(fDT);
}