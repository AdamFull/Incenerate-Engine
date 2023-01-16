#include "SceneSerializer.h"

#include "Engine.h"
#include "SceneGraph.hpp"

#include "system/filesystem/filesystem.h"

#include "ecs/components/components.h"


using namespace engine;
using namespace engine::ecs;
using namespace engine::game;
using namespace engine::system;
using namespace engine::graphics;
using namespace engine::audio;

entt::entity CSceneLoader::load(const std::filesystem::path& scenepath)
{
	FIncenerateScene scene;
	fs::read_json(scenepath, scene);

	return loadNode(scene.root);
}

void CSceneLoader::save(const entt::entity& root, const std::filesystem::path& scenepath)
{
	FIncenerateScene scene;
	scene.root = saveNode(root);

	fs::write_json(scenepath, scene, 2);
}

entt::entity CSceneLoader::loadNode(FSceneObjectRaw& object)
{
	auto& registry = EGCoordinator;

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

	for(auto& child : object.vChildren)
		scenegraph::attach_child(node, loadNode(child));

	return node;
}

FSceneObjectRaw CSceneLoader::saveNode(const entt::entity& node)
{
	bool bIgnoreChildren{ false };
	auto& registry = EGCoordinator;

	FSceneObjectRaw object;

	auto& hierarchy = registry.get<FHierarchyComponent>(node);
	object.srName = hierarchy.name;

	if (auto transform = registry.try_get<FTransformComponent>(node))
		object.mComponents.emplace("transform", nlohmann::json(*transform));
	if (auto scene = registry.try_get<FSceneComponent>(node))
	{
		object.mComponents.emplace("scene", nlohmann::json(*scene));
		bIgnoreChildren = true;
	}
	if (auto camera = registry.try_get<FCameraComponent>(node))
		object.mComponents.emplace("camera", nlohmann::json(*camera));
	if (auto audio = registry.try_get<FAudioComponent>(node))
		object.mComponents.emplace("audio", nlohmann::json(*audio));
	if (auto environment = registry.try_get<FEnvironmentComponent>(node))
		object.mComponents.emplace("environment", nlohmann::json(*environment));
	if (auto sprite = registry.try_get<FSpriteComponent>(node))
		object.mComponents.emplace("sprite", nlohmann::json(*sprite));
	if (auto directionallight = registry.try_get<FDirectionalLightComponent>(node))
		object.mComponents.emplace("directionallight", nlohmann::json(*directionallight));
	if (auto pointlight = registry.try_get<FPointLightComponent>(node))
		object.mComponents.emplace("pointlight", nlohmann::json(*pointlight));
	if (auto spotlight = registry.try_get<FSpotLightComponent>(node))
		object.mComponents.emplace("spotlight", nlohmann::json(*spotlight));
	if (auto script = registry.try_get<FScriptComponent>(node))
		object.mComponents.emplace("script", nlohmann::json(*script));

	for (auto& child : hierarchy.children)
		object.vChildren.emplace_back(saveNode(child));

	return object;
}