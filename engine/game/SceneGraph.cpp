#include "SceneGraph.hpp"

#include "Engine.h"

using namespace engine::ecs;
using namespace engine::game;

#include "ecs/components/components.h"

entt::entity scenegraph::create_node(const std::string& name)
{
	auto& registry = EGEngine->getRegistry();

	auto entity = registry.create();
	registry.emplace<FTransformComponent>(entity, FTransformComponent{});
	registry.emplace<FHierarchyComponent>(entity, FHierarchyComponent{ name });
	log_debug("Entity with id {} was created", static_cast<uint32_t>(entity));
	return entity;
}

void scenegraph::destroy_node(entt::entity node)
{
	auto& registry = EGEngine->getRegistry();
	
	auto& hierarchy = registry.get<FHierarchyComponent>(node);

	while (!hierarchy.children.empty())
	{
		auto child = hierarchy.children.front();
		destroy_node(child);
	}

	detach_child(node);
	hierarchy.children.clear();

	registry.destroy(node);

	log_debug("Entity with id {} was destroyed", static_cast<uint32_t>(node));
}

void scenegraph::attach_child(entt::entity parent, entt::entity child)
{
	if (parent == child)
	{
		log_warning("Trying to add parent to parent. Parent entity {}, child entity {}.", static_cast<uint32_t>(parent), static_cast<uint32_t>(child));
		return;
	}

	auto& registry = EGEngine->getRegistry();

	auto& phierarchy = registry.get<FHierarchyComponent>(parent);
	phierarchy.children.emplace_back(child);

	auto& chierarchy = registry.get<FHierarchyComponent>(child);
	chierarchy.parent = parent;
}

void scenegraph::detach_child(entt::entity parent, entt::entity child)
{
	if (parent == entt::null)
		return;

	auto& registry = EGEngine->getRegistry();

	auto& phierarchy = registry.get<FHierarchyComponent>(parent);
	auto cit = std::find(phierarchy.children.begin(), phierarchy.children.end(), child);

	if (cit != phierarchy.children.end())
		phierarchy.children.erase(cit);
	else
		log_error("Entity with is {} is not child of {}", static_cast<uint32_t>(child), static_cast<uint32_t>(parent));

	auto& chierarchy = registry.get<FHierarchyComponent>(child);
	chierarchy.parent = entt::null;
}

void scenegraph::detach_child(entt::entity child)
{
	auto& registry = EGEngine->getRegistry();
	auto& chierarchy = registry.get<FHierarchyComponent>(child);

	detach_child(chierarchy.parent, child);
}

entt::entity scenegraph::duplicate_node(entt::entity entity)
{
	auto& registry = EGEngine->getRegistry();

	auto& transform = registry.get<FTransformComponent>(entity);
	auto& hierarchy = registry.get<FHierarchyComponent>(entity);

	auto new_node = create_node(hierarchy.name);
	registry.replace<FTransformComponent>(new_node, transform);

	if (auto audio = registry.try_get<FAudioComponent>(entity))
		registry.emplace<FAudioComponent>(new_node, *audio);

	// Skybox can be only one!

	if (auto audio = registry.try_get<FAudioComponent>(entity))
		registry.emplace<FAudioComponent>(new_node, *audio);

	if (auto mesh = registry.try_get<FMeshComponent>(entity))
		registry.emplace<FMeshComponent>(new_node, *mesh);

	if (auto sprite = registry.try_get<FSpriteComponent>(entity))
		registry.emplace<FSpriteComponent>(new_node, *sprite);

	if (auto camera = registry.try_get<FCameraComponent>(entity))
	{
		registry.emplace<FCameraComponent>(new_node, *camera);
		auto& ncamera = registry.get<FCameraComponent>(new_node);
		ncamera.active = false;
	}

	if (auto script = registry.try_get<FScriptComponent>(entity))
		registry.emplace<FScriptComponent>(new_node, *script);

	if (auto light = registry.try_get<FDirectionalLightComponent>(entity))
		registry.emplace<FDirectionalLightComponent>(new_node, *light);

	if (auto light = registry.try_get<FPointLightComponent>(entity))
		registry.emplace<FPointLightComponent>(new_node, *light);

	if (auto light = registry.try_get<FSpotLightComponent>(entity))
		registry.emplace<FSpotLightComponent>(new_node, *light);

	for (auto& child : hierarchy.children)
		attach_child(new_node, duplicate_node(child));

	return new_node;
}

void scenegraph::parent_exchange(entt::entity new_parent, entt::entity child)
{
	auto& registry = EGEngine->getRegistry();
	auto& chierarchy = registry.get<FHierarchyComponent>(child);
	detach_child(chierarchy.parent, child);
	attach_child(new_parent, child);
}