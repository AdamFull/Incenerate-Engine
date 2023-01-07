#include "Engine.h"
#include "system/filesystem/filesystem.h"

#include <utility/utime.hpp>

#include "game/SceneSerializer.h"

using namespace engine;
using namespace engine::ecs;
using namespace engine::game;
using namespace engine::graphics;
using namespace engine::system;
using namespace engine::system::window;

CEngine::CEngine()
{
	log_add_file_output("engine_log.log");
	//log_add_wincmd_output();
	log_add_cout_output();
	log_init("voxel_app", "1.0.0");
}

CEngine::~CEngine()
{
	vSystems.clear();
	pRoot = nullptr;
	pGraphics = nullptr;
	pWindow = nullptr;
	log_debug("Engine was shutdown.");
}

void CEngine::create()
{
	utl::stopwatch sw;
	log_info("Beginning engine initialization.");

	FEngineCreateInfo createInfo;
	fs::read_json("engine/config.cfg", createInfo);

	pEventManager = std::make_unique<CEventManager>();

	pWindow = std::make_unique<CWindowHandle>();
	pWindow->create(createInfo.window);

	pGraphics = std::make_unique<CAPIHandle>(pWindow.get());
	pGraphics->create(createInfo);

	initEntityComponentSystem();

	pRoot = CSceneLoader::load("scene.json");

	for (const auto& system : vSystems)
		system->create();

	auto currentTime = std::chrono::high_resolution_clock::now();
	log_info("Engine initialization finished with: {}s.", sw.stop<float>());

	
}

void CEngine::beginEngineLoop()
{
	utl::stopwatch sw;
	float dt{ 0.f };

	while (pWindow->begin())
	{
		pGraphics->begin();
		for (const auto& system : vSystems)
			system->update(dt);
		pGraphics->end();

		dt = sw.stop<float>();
	}

	pGraphics->shutdown();
}

entt::registry& CEngine::getCoordinator()
{
	return coordinator;
}

const winptr_t& CEngine::getWindow() const
{
	return pWindow;
}

const graphptr_t& CEngine::getGraphics() const
{
	return pGraphics;
}

const scenegraph_t& CEngine::getSceneGraph() const
{
	return pRoot;
}

void CEngine::sendEvent(CEvent& event)
{
	pEventManager->sendEvent(event);
}

void CEngine::sendEvent(EventId eventId)
{
	pEventManager->sendEvent(eventId);
}