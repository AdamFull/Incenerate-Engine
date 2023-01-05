#include "Engine.h"

// Components
#include "ecs/components/AudioComponent.h"
#include "ecs/components/MeshComponent.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/components/SpriteComponent.h"
#include "ecs/components/DirectionalLightComponent.h"
#include "ecs/components/PointLightComponent.h"
#include "ecs/components/SpotLightComponent.h"
#include "ecs/components/SkyboxComponent.h"
#include "ecs/components/ScriptComponent.h"

// Systems
#include "ecs/systems/AudioSystem.h"
#include "ecs/systems/3DRenderSystem.h"
#include "ecs/systems/2DRenderSystem.h"
#include "ecs/systems/CompositionSystem.h"
#include "ecs/systems/CameraControlSystem.h"
#include "ecs/systems/InputSystem.h"
#include "ecs/systems/ScriptingSystem.h"

using namespace engine;
using namespace engine::ecs;

void CEngine::initEntityComponentSystem()
{
	pCoordinator->registerComponent<FAudioComponent>();
	pCoordinator->registerComponent<FMeshComponent>();
	pCoordinator->registerComponent<FTransformComponent>();
	pCoordinator->registerComponent<FCameraComponent>();
	pCoordinator->registerComponent<FSpriteComponent>();
	pCoordinator->registerComponent<FDirectionalLightComponent>();
	pCoordinator->registerComponent<FPointLightComponent>();
	pCoordinator->registerComponent<FSpotLightComponent>();
	pCoordinator->registerComponent<FSkyboxComponent>();
	pCoordinator->registerComponent<FScriptComponent>();

	auto pInputSystem = pCoordinator->registerSystem<CInputSystem>();
	vSystems.emplace_back(pInputSystem);

	auto pScriptingSystem = pCoordinator->registerSystem<CScriptingSystem>();
	{
		Signature sign;
		sign.set(pCoordinator->getComponentType<FScriptComponent>());
		pCoordinator->setSystemSignature<CScriptingSystem>(sign);
	}
	vSystems.emplace_back(pScriptingSystem);

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

	auto pCompositionSystem = pCoordinator->registerSystem<CCompositionSystem>();
	{
		Signature sign;
		sign.set(pCoordinator->getComponentType<FDirectionalLightComponent>());
		sign.set(pCoordinator->getComponentType<FPointLightComponent>());
		sign.set(pCoordinator->getComponentType<FSpotLightComponent>());
		pCoordinator->setSystemSignature<CCompositionSystem>(sign);
	}
	vSystems.emplace_back(pCompositionSystem);

	//auto p2DRenderSystem = pCoordinator->registerSystem<C2DRenderSystem>();
	//{
	//	Signature sign;
	//	sign.set(pCoordinator->getComponentType<FSpriteComponent>());
	//	sign.set(pCoordinator->getComponentType<FTransformComponent>());
	//	pCoordinator->setSystemSignature<C2DRenderSystem>(sign);
	//}
	//vSystems.emplace_back(p2DRenderSystem);
}