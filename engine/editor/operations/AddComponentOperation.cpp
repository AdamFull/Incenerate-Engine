#include "AddComponentOperation.h"

#include "Engine.h"
#include "ecs/components/components.h"

using namespace engine::ecs;
using namespace engine::game;
using namespace engine::editor;

CAddComponentOperation::CAddComponentOperation(entt::entity entity, size_t component_id)
{
	this->target = entity;
	this->component_id = component_id;
	redo();
}

void CAddComponentOperation::undo()
{
	auto& registry = EGEngine->getRegistry();

	switch (component_id)
	{
	case component::audio: { registry.remove<FAudioComponent>(target); } break;
	case component::camera: { registry.remove<FCameraComponent>(target); } break;
	case component::directional_light: { registry.remove<FDirectionalLightComponent>(target); } break;
	case component::environment: { registry.remove<FEnvironmentComponent>(target); } break;
	case component::point_light: { registry.remove<FPointLightComponent>(target); } break;
	case component::scene: { registry.remove<FSceneComponent>(target); } break;
	case component::script: { registry.remove<FScriptComponent>(target); } break;
	case component::spot_light: { registry.remove<FSpotLightComponent>(target); } break;
	case component::sprite: { registry.remove<FSpriteComponent>(target); } break;
	case component::rigidbody: { registry.remove<FRigidBodyComponent>(target); } break;
	case component::collider: { registry.remove<FColliderComponent>(target); } break;
	case component::particle: { registry.remove<FParticleComponent>(target); } break;
	case component::terrain: { registry.remove<FTerrainComponent>(target); } break;
	default:
		break;
	}
}

void CAddComponentOperation::redo()
{
	auto& registry = EGEngine->getRegistry();

	switch (component_id)
	{
	case component::audio: { registry.emplace<FAudioComponent>(target, FAudioComponent{}); } break;
	case component::camera: { registry.emplace<FCameraComponent>(target, FCameraComponent{}); } break;
	case component::directional_light: { registry.emplace<FDirectionalLightComponent>(target, FDirectionalLightComponent{}); } break;
	case component::environment: { registry.emplace<FEnvironmentComponent>(target, FEnvironmentComponent{}); } break;
	case component::point_light: { registry.emplace<FPointLightComponent>(target, FPointLightComponent{}); } break;
	case component::scene: { registry.emplace<FSceneComponent>(target, FSceneComponent{}); } break;
	case component::script: { registry.emplace<FScriptComponent>(target, FScriptComponent{}); } break;
	case component::spot_light: { registry.emplace<FSpotLightComponent>(target, FSpotLightComponent{}); } break;
	case component::sprite: { registry.emplace<FSpriteComponent>(target, FSpriteComponent{}); } break;
	case component::rigidbody: { registry.emplace<FRigidBodyComponent>(target, FRigidBodyComponent{}); } break;
	case component::collider: { registry.emplace<FColliderComponent>(target, FColliderComponent{}); } break;
	case component::particle: { registry.emplace<FParticleComponent>(target, FParticleComponent{}); } break;
	case component::terrain: { registry.emplace<FTerrainComponent>(target, FTerrainComponent{}); } break;
	default:
		break;
	}
}