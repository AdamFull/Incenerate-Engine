#include "SceneSerializer.h"

#include "Engine.h"
#include "SceneGraph.hpp"

#include "filesystem/vfs_helper.h"

#include "ecs/components/components.h"


using namespace engine;
using namespace engine::ecs;
using namespace engine::game;
using namespace engine::filesystem;
using namespace engine::graphics;
using namespace engine::audio;

entt::entity CSceneLoader::load(const std::string& scenepath)
{
	FIncenerateScene scene;
	EGFilesystem->readBson(scenepath, scene);
	return loadNode(fs::get_mount_point(scenepath), scene.root);
}

void CSceneLoader::save(const entt::entity& root, const std::string& scenepath)
{
	FIncenerateScene scene;
	scene.root = saveNode(root);

	EGFilesystem->writeBson(scenepath, scene);
}

entt::entity CSceneLoader::loadNode(const std::string& mountpoint, FSceneObjectRaw& object)
{
	auto& registry = EGEngine->getRegistry();

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
		{
			auto environment = component.get<FEnvironmentComponent>();
			environment.source = fs::to_posix_path(fs::path_append(mountpoint, environment.source));
			registry.emplace<FEnvironmentComponent>(node, environment);
		}

		if (name == "sprite")
		{
			auto sprite = component.get<FSpriteComponent>();
			sprite.source = fs::to_posix_path(fs::path_append(mountpoint, sprite.source));
			registry.emplace<FSpriteComponent>(node, sprite);
		}

		if (name == "scene")
		{
			auto scene = component.get<FSceneComponent>();
			scene.source = fs::to_posix_path(fs::path_append(mountpoint, scene.source));
			registry.emplace<FSceneComponent>(node, scene);
		}

		if (name == "directionallight")
			registry.emplace<FDirectionalLightComponent>(node, component.get<FDirectionalLightComponent>());

		if (name == "pointlight")
			registry.emplace<FPointLightComponent>(node, component.get<FPointLightComponent>());

		if (name == "spotlight")
			registry.emplace<FSpotLightComponent>(node, component.get<FSpotLightComponent>());

		if (name == "script")
		{
			auto script = component.get<FScriptComponent>();
			script.source = fs::to_posix_path(fs::path_append(mountpoint, script.source));
			registry.emplace<FScriptComponent>(node, script);
		}

		if (name == "rigidbody")
			registry.emplace<FRigidBodyComponent>(node, component.get<FRigidBodyComponent>());

		if (name == "terrain")
		{
			auto terrain = component.get<FTerrainComponent>();
			terrain.source = fs::to_posix_path(fs::path_append(mountpoint, terrain.source));
			registry.emplace<FTerrainComponent>(node, terrain);
		}	
	}

	for(auto& child : object.vChildren)
		scenegraph::attach_child(node, loadNode(mountpoint, child));

	return node;
}

FSceneObjectRaw CSceneLoader::saveNode(const entt::entity& node)
{
	bool bIgnoreChildren{ false };
	auto& registry = EGEngine->getRegistry();

	FSceneObjectRaw object;

	auto& hierarchy = registry.get<FHierarchyComponent>(node);
	object.srName = hierarchy.name;

	if (auto transform = registry.try_get<FTransformComponent>(node))
		object.mComponents.emplace("transform", nlohmann::json(*transform));

	if (auto scene = registry.try_get<FSceneComponent>(node))
	{
		FSceneComponent scenecopy = *scene;
		scenecopy.source = fs::get_local_path(scenecopy.source);
		object.mComponents.emplace("scene", nlohmann::json(scenecopy));
		bIgnoreChildren = true;
	}
	if (auto camera = registry.try_get<FCameraComponent>(node))
		object.mComponents.emplace("camera", nlohmann::json(*camera));

	if (auto audio = registry.try_get<FAudioComponent>(node))
	{
		FAudioComponent acopy = *audio;
		acopy.source = fs::get_local_path(acopy.source);
		object.mComponents.emplace("audio", nlohmann::json(acopy));
	}
		
	if (auto environment = registry.try_get<FEnvironmentComponent>(node))
	{
		FEnvironmentComponent envcopy = *environment;
		envcopy.source = fs::get_local_path(envcopy.source);
		object.mComponents.emplace("environment", nlohmann::json(envcopy));
	}
		
	if (auto sprite = registry.try_get<FSpriteComponent>(node))
	{
		FSpriteComponent sptcopy = *sprite;
		sptcopy.source = fs::get_local_path(sptcopy.source);
		object.mComponents.emplace("sprite", nlohmann::json(sptcopy));
	}
		
	if (auto directionallight = registry.try_get<FDirectionalLightComponent>(node))
		object.mComponents.emplace("directionallight", nlohmann::json(*directionallight));

	if (auto pointlight = registry.try_get<FPointLightComponent>(node))
		object.mComponents.emplace("pointlight", nlohmann::json(*pointlight));

	if (auto spotlight = registry.try_get<FSpotLightComponent>(node))
		object.mComponents.emplace("spotlight", nlohmann::json(*spotlight));

	if (auto script = registry.try_get<FScriptComponent>(node))
	{
		FScriptComponent scrptcopy = *script;
		scrptcopy.source = fs::get_local_path(scrptcopy.source);
		object.mComponents.emplace("script", nlohmann::json(scrptcopy));
	}
		
	if (auto rigidbody = registry.try_get<FRigidBodyComponent>(node))
		object.mComponents.emplace("rigidbody", nlohmann::json(*rigidbody));

	if (auto terrain = registry.try_get<FTerrainComponent>(node))
	{
		FTerrainComponent terraincopy = *terrain;
		terraincopy.source = fs::get_local_path(terraincopy.source);
		object.mComponents.emplace("terrain", nlohmann::json(terraincopy));
	}		

	if (bIgnoreChildren)
		return object;

	for (auto& child : hierarchy.children)
		object.vChildren.emplace_back(saveNode(child));

	return object;
}

void CSceneLoader::saveComponents(std::map<std::string, std::any>& components, const entt::entity& node)
{
	auto& registry = EGEngine->getRegistry();

	if (auto transform = registry.try_get<FTransformComponent>(node))
		components.emplace("transform", *transform);
	if (auto scene = registry.try_get<FSceneComponent>(node))
		components.emplace("scene", *scene);
	if (auto camera = registry.try_get<FCameraComponent>(node))
		components.emplace("camera", *camera);
	if (auto audio = registry.try_get<FAudioComponent>(node))
		components.emplace("audio", *audio);
	if (auto environment = registry.try_get<FEnvironmentComponent>(node))
		components.emplace("environment", *environment);
	if (auto sprite = registry.try_get<FSpriteComponent>(node))
		components.emplace("sprite", *sprite);
	if (auto directionallight = registry.try_get<FDirectionalLightComponent>(node))
		components.emplace("directionallight", *directionallight);
	if (auto pointlight = registry.try_get<FPointLightComponent>(node))
		components.emplace("pointlight", *pointlight);
	if (auto spotlight = registry.try_get<FSpotLightComponent>(node))
		components.emplace("spotlight", *spotlight);
	if (auto script = registry.try_get<FScriptComponent>(node))
		components.emplace("script", *script);
	if (auto script = registry.try_get<FRigidBodyComponent>(node))
		components.emplace("rigidbody", *script);
}

void CSceneLoader::applyComponents(const std::map<std::string, std::any>& components, const entt::entity& node)
{
	auto& registry = EGEngine->getRegistry();

	for (auto& [name, component] : components)
	{
		if (name == "transform")
			registry.emplace_or_replace<FTransformComponent>(node, std::any_cast<FTransformComponent>(component));
		if (name == "camera")
			registry.emplace_or_replace<FCameraComponent>(node, std::any_cast<FCameraComponent>(component));
		if (name == "audio")
			registry.emplace_or_replace<FAudioComponent>(node, std::any_cast<FAudioComponent>(component));
		if (name == "environment")
			registry.emplace_or_replace<FEnvironmentComponent>(node, std::any_cast<FEnvironmentComponent>(component));
		if (name == "sprite")
			registry.emplace_or_replace<FSpriteComponent>(node, std::any_cast<FSpriteComponent>(component));
		if (name == "scene")
			registry.emplace_or_replace<FSceneComponent>(node, std::any_cast<FSceneComponent>(component));
		if (name == "directionallight")
			registry.emplace_or_replace<FDirectionalLightComponent>(node, std::any_cast<FDirectionalLightComponent>(component));
		if (name == "pointlight")
			registry.emplace_or_replace<FPointLightComponent>(node, std::any_cast<FPointLightComponent>(component));
		if (name == "spotlight")
			registry.emplace_or_replace<FSpotLightComponent>(node, std::any_cast<FSpotLightComponent>(component));
		if (name == "script")
			registry.emplace_or_replace<FScriptComponent>(node, std::any_cast<FScriptComponent>(component));
		if (name == "rigidbody")
			registry.emplace_or_replace<FRigidBodyComponent>(node, std::any_cast<FRigidBodyComponent>(component));
	}
}