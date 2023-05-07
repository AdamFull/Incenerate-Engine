#include "Engine.h"
#include "filesystem/vfs_helper.h"

#include "game/SceneSerializer.h"

// Audio
#include "audio/AudioCore.h"

// Event manager
#include "event/EventManager.hpp"

// VFS
#include "filesystem/VirtualFileSystemManager.h"
#include "filesystem/native/NativeFileSystem.h"

// Scripting
#include "scripting/ScriptingCore.h"

#include <SessionStorage.hpp>


using namespace engine;
using namespace engine::game;
using namespace engine::graphics;
using namespace engine::audio;
using namespace engine::scripting;
using namespace engine::physics;
//using namespace engine::particles;
using namespace engine::filesystem;

CEngine::CEngine()
{
	log_add_file_output("engine_log.log");
	//log_add_wincmd_output();
	log_add_cout_output();
	log_init("voxel_app", "1.0.0");
}

CEngine::~CEngine()
{
	
}

void CEngine::initialize()
{
	auto local_path = std::filesystem::current_path() / "embed";
	auto temp_path = std::filesystem::temp_directory_path() / "incenerate-engine";

	pFilesystem = std::make_unique<CVirtualFileSystemManager>();
	pFilesystem->mount("/embed", std::make_unique<CNativeFileSystem>(local_path.string()));
	pFilesystem->mount("/temp", std::make_unique<CNativeFileSystem>(temp_path.string()));

	pEventManager = std::make_unique<CEventManager>();
	pSceneManager = std::make_unique<CSceneManager>();
	pPhysics = std::make_unique<CPhysicsCore>();
	pAudio = std::make_unique<CAudioCore>(pFilesystem.get());

	pScripting = std::make_unique<CScriptingCore>(pFilesystem.get());
	pGraphics = std::make_unique<CAPIHandle>();
}

void CEngine::create()
{
	utl::stopwatch sw;
	log_info("Beginning engine initialization.");

	FEngineCreateInfo createInfo;
	pFilesystem->readJson("/embed/config.json", createInfo);

	//pLoaderThread = std::make_unique<utl::threadworker>();

	pPhysics->create();
	pAudio->create();
	pScripting->create();

	pGraphics->setVirtualFileSystem(pFilesystem.get());
	pGraphics->setEventSystem(pEventManager.get());
	pGraphics->create(createInfo);

	// Load it from config?
	bEditorMode = CSessionStorage::getInstance()->get<bool>("editor_mode");
	if (bEditorMode)
		eState = EEngineState::eEditing;

	initEntityComponentSystem();

	for (const auto& system : vSystems)
		system->create();

	auto currentTime = std::chrono::high_resolution_clock::now();
	log_info("Engine initialization finished with: {}s.", sw.stop<float>());
}

void CEngine::beginEngineLoop()
{
	utl::stopwatch sw;
	float dt{ 0.f };

	//pLoaderThread->wait();
	bool bRunning{ true };
	while (bRunning)
	{
		auto commandBuffer = pGraphics->begin(bRunning);
		
		if (!commandBuffer)
			continue;

		for (const auto& system : vSystems)
			system->update(dt);

		pGraphics->end(dt);

		pScripting->update();
		pAudio->update(dt);
		pGraphics->update();

		dt = sw.stop<float>();
	}

	destruction();
}

void CEngine::destruction()
{
	vSystems.clear();
	pSceneManager = nullptr;
	
	pAudio->shutdown();
	pGraphics->shutdown();

	log_debug("Engine was shutdown.");
}

entt::registry& CEngine::getRegistry()
{
	return registry;
}

const graphptr_t& CEngine::getGraphics() const
{
	return pGraphics;
}

const audiocore_t& CEngine::getAudio() const
{
	return pAudio;
}

const scenemgr_t& CEngine::getSceneManager() const
{
	return pSceneManager;
}

const scriptcore_t& CEngine::getScripting() const
{
	return pScripting;
}

const physicscore_t& CEngine::getPhysics() const
{
	return pPhysics;
}

//const particlescore_t& CEngine::getParticles() const
//{
//	return pParticles;
//}

const filesystem_t& CEngine::getFilesystem() const
{
	return pFilesystem;
}

const std::unique_ptr<utl::threadworker>& CEngine::getLoaderThread()
{
	return pLoaderThread;
}

const bool CEngine::isEditorEditing() const
{
	return bEditorMode && eState == EEngineState::eEditing;
}

const std::vector<std::unique_ptr<ecs::ISystem>>& CEngine::getSystems() const
{
	return vSystems;
}

std::unique_ptr<IEvent> CEngine::makeEvent(EventId eventId)
{
	return pEventManager->makeEvent(eventId);
}

void CEngine::sendEvent(const std::unique_ptr<IEvent>& event)
{
	pEventManager->sendEvent(event);
}

void CEngine::sendEvent(EventId eventId)
{
	pEventManager->sendEvent(eventId);
}