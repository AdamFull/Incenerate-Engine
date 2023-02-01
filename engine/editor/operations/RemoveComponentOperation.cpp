#include "RemoveComponentOperation.h"

#include "Engine.h"
#include "ecs/components/components.h"

using namespace engine::ecs;
using namespace engine::game;
using namespace engine::editor;

CRemoveComponentOperation::CRemoveComponentOperation(entt::entity entity, size_t component_id)
{
	this->target = entity;
	this->component_id = component_id;
	redo();
}

void CRemoveComponentOperation::undo()
{
	auto& registry = EGEngine->getRegistry();

	switch (component_id)
	{
	case component::audio: 
	{ 
		auto pComp = std::any_cast<FAudioComponent>(component);
		pComp.loaded = false;

		registry.emplace<FAudioComponent>(target, pComp);
	} break;

	case component::camera: 
	{ 
		auto pComp = std::any_cast<FCameraComponent>(component);
		registry.emplace<FCameraComponent>(target, pComp);
	} break;

	case component::directional_light: 
	{ 
		auto pComp = std::any_cast<FDirectionalLightComponent>(component);
		registry.emplace<FDirectionalLightComponent>(target, pComp); 
	} break;

	case component::environment: 
	{ 
		auto pComp = std::any_cast<FEnvironmentComponent>(component);
		pComp.loaded = false;

		registry.emplace<FEnvironmentComponent>(target, pComp);
	} break;

	case component::point_light: 
	{ 
		auto pComp = std::any_cast<FPointLightComponent>(component);
		registry.emplace<FPointLightComponent>(target, pComp);
	} break;

	case component::scene: 
	{ 
		auto pComp = std::any_cast<FSceneComponent>(component);
		pComp.loaded = false;

		registry.emplace<FSceneComponent>(target, pComp);
	} break;

	case component::script: 
	{ 
		auto pComp = std::any_cast<FScriptComponent>(component);
		pComp.loaded = false;

		registry.emplace<FScriptComponent>(target, pComp);
	} break;

	case component::spot_light: 
	{ 
		auto pComp = std::any_cast<FSpotLightComponent>(component);
		registry.emplace<FSpotLightComponent>(target, pComp);
	} break;

	case component::sprite: 
	{ 
		auto pComp = std::any_cast<FSpriteComponent>(component);
		registry.emplace<FSpriteComponent>(target, pComp);
	} break;
	case component::rigidbody:
	{
		auto pComp = std::any_cast<FRigidBodyComponent>(component);
		registry.emplace<FRigidBodyComponent>(target, pComp);
	} break;

	default:
		break;
	}

	EGEditor->selectObject(target);
}

void CRemoveComponentOperation::redo()
{
	auto& registry = EGEngine->getRegistry();

	switch (component_id)
	{
	case component::audio: 
	{ 
		component = registry.get<FAudioComponent>(target);
		registry.remove<FAudioComponent>(target); 
	} break;

	case component::camera: 
	{ 
		component = registry.get<FCameraComponent>(target);
		registry.remove<FCameraComponent>(target); 
	} break;

	case component::directional_light: 
	{ 
		component = registry.get<FDirectionalLightComponent>(target);
		registry.remove<FDirectionalLightComponent>(target); 
	} break;

	case component::environment: 
	{ 
		component = registry.get<FEnvironmentComponent>(target);
		registry.remove<FEnvironmentComponent>(target); 
	} break;

	case component::point_light: 
	{ 
		component = registry.get<FPointLightComponent>(target);
		registry.remove<FPointLightComponent>(target); 
	} break;

	case component::scene: 
	{ 
		component = registry.get<FSceneComponent>(target);
		registry.remove<FSceneComponent>(target); 
	} break;

	case component::script: 
	{ 
		component = registry.get<FScriptComponent>(target);
		registry.remove<FScriptComponent>(target); 
	} break;

	case component::spot_light: 
	{ 
		component = registry.get<FSpotLightComponent>(target);
		registry.remove<FSpotLightComponent>(target); 
	} break;

	case component::sprite: 
	{ 
		component = registry.get<FSpriteComponent>(target);
		registry.remove<FSpriteComponent>(target); 
	} break;
	case component::rigidbody:
	{
		component = registry.get<FRigidBodyComponent>(target);
		registry.remove<FRigidBodyComponent>(target);
	} break;
	default:
		break;
	}

	EGEditor->deselectAll();
}