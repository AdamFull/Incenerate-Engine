#include "RemoveEntityOperation.h"

#include "Engine.h"
#include "game/SceneGraph.hpp"
#include "game/SceneSerializer.h"
#include "ecs/components/components.h"

using namespace engine::ecs;
using namespace engine::game;
using namespace engine::editor;

CRemoveEntityOperation::CRemoveEntityOperation(entt::entity entity)
{
	target = entity;
	redo();
}

void CRemoveEntityOperation::undo()
{
	target = CSceneLoader::loadNode(serialized);
	scenegraph::attach_child(parent, target);
	EGEditor->selectObject(target);
}

void CRemoveEntityOperation::redo()
{
	auto& registry = EGEngine->getRegistry();
	auto& hierarchy = registry.get<FHierarchyComponent>(target);
	parent = hierarchy.parent;

	serialized = CSceneLoader::saveNode(target);
	scenegraph::destroy_node(target);
	EGEditor->deselectAll();
}