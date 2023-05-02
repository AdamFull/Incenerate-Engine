#include "Engine.h"
#include "filesystem/vfs_helper.h"

#include "game/SceneSerializer.h"
#include "graphics/window/SDL2WindowAdapter.h"

#include "filesystem/native/NativeFileSystem.h"

using namespace engine;
using namespace engine::editor;
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

void CEngine::create()
{
	utl::stopwatch sw;
	log_info("Beginning engine initialization.");

	auto local_path = std::filesystem::current_path() / "embed";
	auto temp_path = std::filesystem::temp_directory_path() / "incenerate-engine";

	pFilesystem = std::make_unique<CVirtualFileSystemManager>();
	pFilesystem->mount("/embed", std::make_unique<CNativeFileSystem>(local_path.string()));
	pFilesystem->mount("/temp", std::make_unique<CNativeFileSystem>(temp_path.string()));

	FEngineCreateInfo createInfo;
	pFilesystem->readJson("/embed/config.json", createInfo);

	//pLoaderThread = std::make_unique<utl::threadworker>();

	pEventManager = std::make_unique<CEventManager>();

	pSceneManager = std::make_unique<CSceneManager>();

	pPhysics = std::make_unique<CPhysicsCore>();
	pPhysics->create();

	pAudio = std::make_unique<CAudioCore>();
	pAudio->create();

	pScripting = std::make_unique<CScriptingCore>(pFilesystem.get());
	pScripting->create();

	pWindow = std::make_unique<CSDL2WindowAdapter>();
	pWindow->create(createInfo.window);

	pGraphics = std::make_unique<CAPIHandle>(pWindow.get());
	pGraphics->setVirtualFileSystem(pFilesystem.get());
	pGraphics->create(createInfo);

	//pParticles = std::make_unique<CParticlesCore>();
	//pParticles->create();

	pEditor = std::make_unique<CEditor>();

	// Load it from config?
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
	while (pWindow->processEvents())
	{
		auto commandBuffer = pGraphics->begin();
		
		if (!commandBuffer)
			continue;

		for (const auto& system : vSystems)
			system->update(dt);

		pGraphics->end(dt);

		pScripting->update();
		pAudio->update();
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

	pEditor = nullptr;

	log_debug("Engine was shutdown.");
}

entt::registry& CEngine::getRegistry()
{
	return registry;
}

const winptr_t& CEngine::getWindow() const
{
	return pWindow;
}

const graphptr_t& CEngine::getGraphics() const
{
	return pGraphics;
}

const editorptr_t& CEngine::getEditor() const
{
	return pEditor;
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

const bool CEngine::isEditorMode() const
{
	return bEditorMode;
}

const bool CEngine::isEditorEditing() const
{
	return bEditorMode && eState == EEngineState::eEditing;
}

const bool CEngine::isDebugDrawNormals() const
{
	return bDebugDrawNormals;
}

const bool CEngine::isDebugDrawPhysics() const
{
	return bDebugDrawPhysics;
}

void CEngine::toggleDebugDrawNormals()
{
	bDebugDrawNormals = !bDebugDrawNormals;
}

void CEngine::toggleDebugDrawPhysics()
{
	bDebugDrawPhysics = !bDebugDrawPhysics;
}

const std::vector<std::unique_ptr<ecs::ISystem>>& CEngine::getSystems() const
{
	return vSystems;
}

void CEngine::sendEvent(CEvent& event)
{
	pEventManager->sendEvent(event);
}

void CEngine::sendEvent(EventId eventId)
{
	pEventManager->sendEvent(eventId);
}