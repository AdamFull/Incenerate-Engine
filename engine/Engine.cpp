#include "Engine.h"
#include "system/filesystem/filesystem.h"

using namespace engine;
using namespace engine::game;
using namespace engine::graphics;
using namespace engine::system;
using namespace engine::system::window;
using namespace engine::system::input;

CEngine::CEngine()
{
	log_add_file_output("engine_log.log");
	//log_add_wincmd_output();
	log_add_cout_output();
	log_init("voxel_app", "1.0.0");
}

void CEngine::create()
{
	auto startTime = std::chrono::high_resolution_clock::now();
	log_info("Beginning engine initialization.");

	FEngineCreateInfo createInfo;
	fs::read_json("engine/config.cfg", createInfo);

	pWindow = std::make_unique<CWindowHandle>();
	pWindow->create(createInfo.window);

	pInputMapper = std::make_unique<CInputMapper>();

	pGraphics = std::make_unique<CAPIHandle>(pWindow.get());
	pGraphics->create(createInfo);

	pScene = std::make_shared<CScene>();
	pScene->create();

	auto currentTime = std::chrono::high_resolution_clock::now();
	log_info("Engine initialization finished with: {}s.", std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count());
}

void CEngine::begin_render_loop()
{
	float delta_time{ 0.001f };
	while (pWindow->begin())
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		pInputMapper->update(delta_time);

		pScene->update(delta_time);

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