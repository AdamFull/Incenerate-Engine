#include "Engine.h"

// Components
#include "ecs/components/AudioComponent.h"
#include "ecs/components/MeshComponent.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/components/LightComponent.h"

// Systems
#include "ecs/systems/AudioSystem.h"
#include "ecs/systems/RenderSystem.h"
#include "ecs/systems/CompositionSystem.h"
#include "ecs/systems/CameraControlSystem.h"

using namespace engine;
using namespace engine::ecs;

void CEngine::initEntityComponentSystem()
{
	pCoordinator->registerComponent<FAudioComponent>();
	pCoordinator->registerComponent<FMeshComponent>();
	pCoordinator->registerComponent<FTransformComponent>();
	pCoordinator->registerComponent<FCameraComponent>();
	pCoordinator->registerComponent<FLightComponent>();

	auto& pCameraControlSystem = pCoordinator->registerSystem<CCameraControlSystem>();
	{
		Signature sign;
		sign.set(pCoordinator->getComponentType<FCameraComponent>());
		sign.set(pCoordinator->getComponentType<FTransformComponent>());
		pCoordinator->setSystemSignature<CCameraControlSystem>(sign);
	}
	vSystems.emplace_back(pCameraControlSystem.get());

	auto& pAudioSystem = pCoordinator->registerSystem<CAudioSystem>();
	{
		Signature sign;
		sign.set(pCoordinator->getComponentType<FAudioComponent>());
		sign.set(pCoordinator->getComponentType<FTransformComponent>());
		pCoordinator->setSystemSignature<CAudioSystem>(sign);
	}
	vSystems.emplace_back(pAudioSystem.get());

	auto& pRenderSystem = pCoordinator->registerSystem<CRenderSystem>();
	{
		Signature sign;
		sign.set(pCoordinator->getComponentType<FMeshComponent>());
		sign.set(pCoordinator->getComponentType<FTransformComponent>());
		pCoordinator->setSystemSignature<CRenderSystem>();
	}
	vSystems.emplace_back(pRenderSystem.get());

	for (const auto& system : vSystems)
		system->create();
}