#include "Engine.h"

#include <chrono>

using namespace engine;
using namespace engine::graphics;
using namespace engine::system::window;
using namespace engine::system::input;

void CEngine::create(const FEngineCreateInfo& createInfo)
{
	pWindow = std::make_unique<CWindowHandle>();
	pWindow->create(createInfo.window);

	pInputMapper = std::make_unique<CInputMapper>();

	pCameraController = std::make_unique<CFreeCameraController>();
	pCameraController->create(pInputMapper.get());

	auto& camera = pCameraController->getCamera();
	camera->setAspect(pWindow->getAspect());

	pGraphics = std::make_unique<CAPIHandle>();
	pGraphics->create(createInfo);
}

void CEngine::begin_render_loop()
{
	float delta_time{ 0.001f };
	while (pWindow->begin())
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		pInputMapper->update(delta_time);
		pCameraController->update(delta_time);

		pGraphics->render();

		pWindow->end();

		auto currentTime = std::chrono::high_resolution_clock::now();
		delta_time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	}
}

const winptr_t& CEngine::getWindow() const
{
	return pWindow;
}

const inputptr_t& CEngine::getInputMapper() const
{
	return pInputMapper;
}

const graphptr_t& CEngine::getGraphics() const
{
	return pGraphics;
}

const camctrlptr_t& CEngine::getCameraController() const
{
	return pCameraController;
}