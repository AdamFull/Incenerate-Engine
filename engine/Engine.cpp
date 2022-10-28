#include "Engine.h"

using namespace engine;
using namespace engine::graphics;
using namespace engine::system::window;
using namespace engine::system::input;

CEngine::CEngine()
{
	log_add_file_output("engine_log.log");
	//log_add_wincmd_output();
	log_add_cout_output();
	log_init("voxel_app", "1.0.0");
}

void CEngine::create(const FEngineCreateInfo& createInfo)
{
	auto startTime = std::chrono::high_resolution_clock::now();
	log_info("Beginning engine initialization.");

	pWindow = std::make_unique<CWindowHandle>();
	pWindow->create(createInfo.window);

	pInputMapper = std::make_unique<CInputMapper>();

	pCameraController = std::make_unique<CFreeCameraController>();
	pCameraController->create(pInputMapper.get());

	auto& camera = pCameraController->getCamera();
	camera->setAspect(pWindow->getAspect());

	pGraphics = std::make_unique<CAPIHandle>(pWindow.get());
	pGraphics->create(createInfo);

	auto currentTime = std::chrono::high_resolution_clock::now();
	log_info("Engine initialization finished with: {}s.", std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count());
}

void CEngine::begin_render_loop()
{
	float delta_time{ 0.001f };
	while (pWindow->begin())
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		//TODO: change after adding scene system
		auto& camera = pCameraController->getCamera();
		camera->setAspect(pWindow->getAspect());

		pInputMapper->update(delta_time);
		pCameraController->update(delta_time);

		pGraphics->render();

		pWindow->end();

		auto currentTime = std::chrono::high_resolution_clock::now();
		delta_time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	}

	pGraphics->shutdown();
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