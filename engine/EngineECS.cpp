#include "Engine.h"

// Components
#include "ecs/components/AudioComponent.h"
#include "ecs/components/MeshComponent.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/components/LightComponent.h"
#include "ecs/components/SpriteComponent.h"

// Systems
#include "ecs/systems/AudioSystem.h"
#include "ecs/systems/3DRenderSystem.h"
#include "ecs/systems/2DRenderSystem.h"
#include "ecs/systems/CompositionSystem.h"
#include "ecs/systems/CameraControlSystem.h"
#include "ecs/systems/InputSystem.h"

using namespace engine;
using namespace engine::ecs;

void CEngine::initEntityComponentSystem()
{
	pCoordinator = std::make_unique<CCoordinator>();
	pCoordinator->create();

	pCoordinator->registerComponent<FAudioComponent>();
	pCoordinator->registerComponent<FMeshComponent>();
	pCoordinator->registerComponent<FTransformComponent>();
	pCoordinator->registerComponent<FCameraComponent>();
	pCoordinator->registerComponent<FLightComponent>();
	pCoordinator->registerComponent<FSpriteComponent>();

	auto pInputSystem = pCoordinator->registerSystem<CInputSystem>();
	vSystems.emplace_back(pInputSystem);

	auto pCameraControlSystem = pCoordinator->registerSystem<CCameraControlSystem>();
	{
		Signature sign;
		sign.set(pCoordinator->getComponentType<FCameraComponent>());
		sign.set(pCoordinator->getComponentType<FTransformComponent>());
		pCoordinator->setSystemSignature<CCameraControlSystem>(sign);
	}
	vSystems.emplace_back(pCameraControlSystem);

	auto pAudioSystem = pCoordinator->registerSystem<CAudioSystem>();
	{
		Signature sign;
		sign.set(pCoordinator->getComponentType<FAudioComponent>());
		sign.set(pCoordinator->getComponentType<FTransformComponent>());
		pCoordinator->setSystemSignature<CAudioSystem>(sign);
	}
	vSystems.emplace_back(pAudioSystem);

	auto p3DRenderSystem = pCoordinator->registerSystem<C3DRenderSystem>();
	{
		Signature sign;
		sign.set(pCoordinator->getComponentType<FMeshComponent>());
		sign.set(pCoordinator->getComponentType<FTransformComponent>());
		pCoordinator->setSystemSignature<C3DRenderSystem>(sign);
	}
	vSystems.emplace_back(p3DRenderSystem);

	auto p2DRenderSystem = pCoordinator->registerSystem<C2DRenderSystem>();
	{
		Signature sign;
		sign.set(pCoordinator->getComponentType<FSpriteComponent>());
		sign.set(pCoordinator->getComponentType<FTransformComponent>());
		pCoordinator->setSystemSignature<C2DRenderSystem>(sign);
	}
	vSystems.emplace_back(p2DRenderSystem);
}