#include "SceneSerializer.h"

#include "Engine.h"
#include "SceneGraph.hpp"

#include "system/filesystem/filesystem.h"

#include "ecs/components/HierarchyComponent.h"
#include "ecs/components/AudioComponent.h"
#include "ecs/components/MeshComponent.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/components/SpriteComponent.h"
#include "ecs/components/DirectionalLightComponent.h"
#include "ecs/components/PointLightComponent.h"
#include "ecs/components/SpotLightComponent.h"
#include "ecs/components/EnvironmentComponent.h"
#include "ecs/components/ScriptComponent.h"
#include "ecs/components/SceneComponent.h"

namespace engine
{
	namespace game
	{
		void to_json(nlohmann::json& json, const FSceneObjectRaw& type)
		{
			json = nlohmann::json();
			utl::serialize_from("name", json, type.srName, !type.srName.empty());
			utl::serialize_from("components", json, type.mComponents, !type.mComponents.empty());
			utl::serialize_from("children", json, type.vChildren, !type.vChildren.empty());
		}

		void from_json(const nlohmann::json& json, FSceneObjectRaw& type)
		{
			utl::parse_to("name", json, type.srName);
			utl::parse_to("components", json, type.mComponents);
			utl::parse_to("children", json, type.vChildren);
		}
	}
}

using namespace engine::ecs;
using namespace engine::game;
using namespace engine::system;
using namespace engine::graphics;
using namespace engine::audio;

entt::entity CSceneLoader::load(const std::string& scenepath)
{
	std::vector<FSceneObjectRaw> vScene;
	fs::read_json(scenepath, vScene);

	auto root = scenegraph::create_node("root");

	loadNodes(root, vScene);

	return root;
}

void CSceneLoader::save(const entt::entity& root, const std::string& scenepath)
{
	std::vector<FSceneObjectRaw> vScene;

	auto& registry = EGCoordinator;
	auto& hierarchy = registry.get<FHierarchyComponent>(root);

	for (auto& child : hierarchy.children)
		serializeNodes(child, vScene);

	fs::write_json(scenepath, vScene, 2);
}

void CSceneLoader::loadNodes(const entt::entity& parent, const std::vector<FSceneObjectRaw>& vObjects)
{
	auto& registry = EGCoordinator;

	for (auto& object : vObjects)
	{
		auto node = scenegraph::create_node(object.srName);
		
		for (auto& [name, component] : object.mComponents)
		{
			if (name == "transform")
			{
				auto& transform = registry.get<FTransformComponent>(node);
				transform = component.get<FTransformComponent>();
			}

			if (name == "camera")
				registry.emplace<FCameraComponent>(node, component.get<FCameraComponent>());

			if (name == "audio")
				registry.emplace<FAudioComponent>(node, component.get<FAudioComponent>());

			if (name == "environment")
				registry.emplace<FEnvironmentComponent>(node, component.get<FEnvironmentComponent>());
				

			if (name == "sprite")
				registry.emplace<FSpriteComponent>(node, component.get<FSpriteComponent>());

			if (name == "scene")
				registry.emplace<FSceneComponent>(node, component.get<FSceneComponent>());

			if (name == "directionallight")
				registry.emplace<FDirectionalLightComponent>(node, component.get<FDirectionalLightComponent>());

			if (name == "pointlight")
				registry.emplace<FPointLightComponent>(node, component.get<FPointLightComponent>());

			if (name == "spotlight")
				registry.emplace<FSpotLightComponent>(node, component.get<FSpotLightComponent>());

			if (name == "script")
				registry.emplace<FScriptComponent>(node, component.get<FScriptComponent>());
		}

		loadNodes(node, object.vChildren);

		scenegraph::attach_child(parent, node);
	}
}

void CSceneLoader::serializeNodes(const entt::entity& parent, std::vector<FSceneObjectRaw>& vObjects)
{
	bool bIgnoreChildren{ false };
	auto& registry = EGCoordinator;

	FSceneObjectRaw object;

	auto& phierarchy = registry.get<FHierarchyComponent>(parent);
	object.srName = phierarchy.name;

	if (auto transform = registry.try_get<FTransformComponent>(parent))
		object.mComponents.emplace("transform", nlohmann::json(*transform));
	if (auto scene = registry.try_get<FSceneComponent>(parent))
	{
		object.mComponents.emplace("scene", nlohmann::json(*scene));
		bIgnoreChildren = true;
	}
	if (auto camera = registry.try_get<FCameraComponent>(parent))
		object.mComponents.emplace("camera", nlohmann::json(*camera));
	if (auto audio = registry.try_get<FAudioComponent>(parent))
		object.mComponents.emplace("audio", nlohmann::json(*audio));
	if (auto environment = registry.try_get<FEnvironmentComponent>(parent))
		object.mComponents.emplace("environment", nlohmann::json(*environment));
	if (auto sprite = registry.try_get<FSpriteComponent>(parent))
		object.mComponents.emplace("sprite", nlohmann::json(*sprite));
	if (auto directionallight = registry.try_get<FDirectionalLightComponent>(parent))
		object.mComponents.emplace("directionallight", nlohmann::json(*directionallight));
	if (auto pointlight = registry.try_get<FPointLightComponent>(parent))
		object.mComponents.emplace("pointlight", nlohmann::json(*pointlight));
	if (auto spotlight = registry.try_get<FSpotLightComponent>(parent))
		object.mComponents.emplace("spotlight", nlohmann::json(*spotlight));
	if (auto script = registry.try_get<FScriptComponent>(parent))
		object.mComponents.emplace("script", nlohmann::json(*script));

	if (!bIgnoreChildren)
	{
		for (auto& child : phierarchy.children)
			serializeNodes(child, object.vChildren);
	}

	vObjects.emplace_back(object);
}