#include "CreateEntityOperation.h"

#include "Editor.h"
#include "Engine.h"
#include "game/SceneGraph.hpp"
#include "ecs/components/components.h"

using namespace engine::ecs;
using namespace engine::game;
using namespace engine::editor;

CCreateEntityOperation::CCreateEntityOperation(entt::entity parent, size_t component_id)
{
	this->parent = parent;
	this->component_id = component_id;
	redo();
}

void CCreateEntityOperation::undo()
{
	auto& registry = EGEngine->getRegistry();

	auto& hierarchy = registry.get<FHierarchyComponent>(entity);
	name = hierarchy.name;

	scenegraph::destroy_node(entity);
	EGEditor->deselectAll();
}

void CCreateEntityOperation::redo()
{
	auto& registry = EGEngine->getRegistry();

	entity = scenegraph::create_node(name);

	switch (component_id)
	{
	case component::audio: { registry.emplace<FAudioComponent>(entity, FAudioComponent{}); } break;
	case component::camera: { registry.emplace<FCameraComponent>(entity, FCameraComponent{}); } break;
	case component::directional_light: { registry.emplace<FDirectionalLightComponent>(entity, FDirectionalLightComponent{}); } break;
	case component::environment: { registry.emplace<FEnvironmentComponent>(entity, FEnvironmentComponent{}); } break;
	case component::point_light: { registry.emplace<FPointLightComponent>(entity, FPointLightComponent{}); } break;
	case component::scene: { registry.emplace<FSceneComponent>(entity, FSceneComponent{}); } break;
	case component::script: { registry.emplace<FScriptComponent>(entity, FScriptComponent{}); } break;
	case component::spot_light: { registry.emplace<FSpotLightComponent>(entity, FSpotLightComponent{}); } break;
	case component::sprite: { registry.emplace<FSpriteComponent>(entity, FSpriteComponent{}); } break;
	case component::rigidbody: { registry.emplace<FRigidBodyComponent>(entity, FRigidBodyComponent{}); } break;
	case component::collider: { registry.emplace<FColliderComponent>(entity, FColliderComponent{}); } break;
	case component::particle: { registry.emplace<FParticleComponent>(entity, FParticleComponent{}); } break;
	case component::terrain: { registry.emplace<FTerrainComponent>(entity, FTerrainComponent{}); } break;
	default:
		break;
	}

	scenegraph::attach_child(parent, entity);
	EGEditor->selectObject(entity);
}