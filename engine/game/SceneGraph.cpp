#include "SceneGraph.hpp"

#include "Engine.h"

using namespace engine::ecs;
using namespace engine::game;

#include "ecs/components/TransformComponent.h"
#include "ecs/components/HierarchyComponent.h"
#include "ecs/components/AudioComponent.h"
#include "ecs/components/SkyboxComponent.h"
#include "ecs/components/MeshComponent.h"
#include "ecs/components/SpriteComponent.h"

entt::entity scenegraph::create_node(const std::string& name)
{
	auto& registry = EGCoordinator;

	auto entity = registry.create();
	registry.emplace<FTransformComponent>(entity, FTransformComponent{});
	registry.emplace<FHierarchyComponent>(entity, FHierarchyComponent{ name });
	log_debug("Entity with id {} was created", static_cast<uint32_t>(entity));
	return entity;
}

void scenegraph::destroy_node(entt::entity node)
{
	auto& registry = EGCoordinator;
	
	auto& hierarchy = registry.get<FHierarchyComponent>(node);

	for (auto& child : hierarchy.children)
		destroy_node(child);

	if (auto audio = registry.try_get<FAudioComponent>(node))
		EGAudio->removeSource(audio->asource);

	if (auto sky = registry.try_get<FSkyboxComponent>(node))
	{
		EGGraphics->removeImage(sky->origin);
		EGGraphics->removeImage(sky->brdflut);
		EGGraphics->removeImage(sky->irradiance);
		EGGraphics->removeImage(sky->prefiltred);
		EGGraphics->removeVertexBuffer(sky->vbo_id);
	}

	if (auto sprite = registry.try_get<FSpriteComponent>(node))
		EGGraphics->removeImage(sprite->image);

	hierarchy.parent = entt::null;
	hierarchy.children.clear();

	log_debug("Entity with id {} was destroyed", static_cast<uint32_t>(node));
}

void scenegraph::attach_child(entt::entity parent, entt::entity child)
{
	auto& registry = EGCoordinator;

	auto& phierarchy = registry.get<FHierarchyComponent>(parent);
	phierarchy.children.emplace_back(child);

	auto& chierarchy = registry.get<FHierarchyComponent>(child);
	chierarchy.parent = parent;
}

void scenegraph::detach_child(entt::entity parent, entt::entity child)
{
	auto& registry = EGCoordinator;

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
	auto& registry = EGCoordinator;
	auto& chierarchy = registry.get<FHierarchyComponent>(child);

	detach_child(chierarchy.parent, child);
}

void scenegraph::parent_exchange(entt::entity new_parent, entt::entity child)
{
	auto& registry = EGCoordinator;
	auto& chierarchy = registry.get<FHierarchyComponent>(child);
	detach_child(chierarchy.parent, child);
	attach_child(new_parent, child);
}