#include "CreateEntityOperation.h"

#include "Engine.h"
#include "game/SceneGraph.hpp"
#include "ecs/components/components.h"

using namespace engine::ecs;
using namespace engine::game;
using namespace engine::editor;

CCreateEntityOperation::CCreateEntityOperation(entt::entity parent)
{
	this->parent = parent;
	redo();
}

void CCreateEntityOperation::undo()
{
	auto& registry = EGCoordinator;

	auto& hierarchy = registry.get<FHierarchyComponent>(entity);
	name = hierarchy.name;

	scenegraph::destroy_node(entity);
	EGEditor->deselectAll();
}

void CCreateEntityOperation::redo()
{
	entity = scenegraph::create_node(name);
	scenegraph::attach_child(parent, entity);
	EGEditor->selectObject(entity);
}