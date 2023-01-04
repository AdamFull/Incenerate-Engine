#include "Engine.h"
#include "system/filesystem/filesystem.h"

#include <utility/utime.hpp>

#include "ecs/components/CameraComponent.h"
#include "ecs/components/MeshComponent.h"
#include "ecs/components/AudioComponent.h"

using namespace engine;
using namespace engine::ecs;
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

void CEngine::create()
{
	utl::stopwatch sw;
	log_info("Beginning engine initialization.");

	FEngineCreateInfo createInfo;
	fs::read_json("engine/config.cfg", createInfo);

	initEntityComponentSystem();

	pWindow = std::make_unique<CWindowHandle>();
	pWindow->create(createInfo.window);

	pGraphics = std::make_unique<CAPIHandle>(pWindow.get());
	pGraphics->create(createInfo);

	auto currentTime = std::chrono::high_resolution_clock::now();
	log_info("Engine initialization finished with: {}s.", sw.stop<float>());

	pRoot = std::make_unique<CSceneNode>();
	pRoot->setName("root");


	auto pCamera = std::make_unique<CSceneNode>();
	pCamera->setName("camera");
	pCoordinator->addComponent<FCameraComponent>(pCamera->getEntity(), FCameraComponent{});
	pRoot->attach(std::move(pCamera));

	auto pMesh = std::make_unique<CSceneNode>();
	pMesh->setName("audio_model");
	pCoordinator->addComponent<FMeshComponent>(pMesh->getEntity(), FMeshComponent{});
	pCoordinator->addComponent<FAudioComponent>(pMesh->getEntity(), FAudioComponent{});
	pRoot->attach(std::move(pMesh));
}

void CEngine::beginEngineLoop()
{
	for (const auto& system : vSystems)
		system->create();

	utl::stopwatch sw;
	float dt{ 0.f };

	while (pWindow->begin())
	{
		for (const auto& system : vSystems)
			system->update(dt);

		pGraphics->render();

		dt = sw.stop<float>();
	}

	pGraphics->shutdown();
}

const coordinator_t& CEngine::getCoordinator() const
{
	return pCoordinator;
}

const winptr_t& CEngine::getWindow() const
{
	return pWindow;
}

const graphptr_t& CEngine::getGraphics() const
{
	return pGraphics;
}