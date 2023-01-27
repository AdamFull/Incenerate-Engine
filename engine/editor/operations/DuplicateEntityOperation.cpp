#include "DuplicateEntityOperation.h"

#include "Engine.h"
#include "game/SceneGraph.hpp"

using namespace engine::ecs;
using namespace engine::game;
using namespace engine::editor;

CDuplicateEntityOperation::CDuplicateEntityOperation(entt::entity parent, entt::entity entity)
{
	this->target = entity;
	this->parent = parent;
	redo();
}

void CDuplicateEntityOperation::undo()
{
	scenegraph::destroy_node(entity);
	EGEditor->deselectAll();
}

void CDuplicateEntityOperation::redo()
{
	entity = scenegraph::duplicate_node(target);
	scenegraph::attach_child(parent, entity);
	EGEditor->selectObject(entity);
}