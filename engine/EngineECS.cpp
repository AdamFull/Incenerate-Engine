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
#include "ecs/systems/HierarchySystem.h"
#include "ecs/systems/AudioSystem.h"
#include "ecs/systems/rendering/shadows/ShadowPassSystem.h"
#include "ecs/systems/rendering/EnvironmentSystem.h"
#include "ecs/systems/rendering/3DRenderSystem.h"
#include "ecs/systems/rendering/2DRenderSystem.h"
#include "ecs/systems/rendering/CompositionSystem.h"
#include "ecs/systems/rendering/PostProcessSystem.h"
#include "ecs/systems/CameraControlSystem.h"
#include "ecs/systems/InputSystem.h"
#include "ecs/systems/ScriptingSystem.h"

using namespace engine;
using namespace engine::ecs;

void CEngine::initEntityComponentSystem()
{
	vSystems.emplace_back(std::make_unique<CHierarchySystem>());
	vSystems.emplace_back(std::make_unique<CInputSystem>());
	vSystems.emplace_back(std::make_unique<CScriptingSystem>());
	vSystems.emplace_back(std::make_unique<CCameraControlSystem>());
	vSystems.emplace_back(std::make_unique<CAudioSystem>());
	vSystems.emplace_back(std::make_unique<CShadowPassSystem>());
	vSystems.emplace_back(std::make_unique<CEnvironmentSystem>());
	vSystems.emplace_back(std::make_unique<C3DRenderSystem>());
	vSystems.emplace_back(std::make_unique<CCompositionSystem>());
	vSystems.emplace_back(std::make_unique<CPostProcessSystem>());

	//vSystems.emplace_back(std::make_unique<C2DRenderSystem>());
}