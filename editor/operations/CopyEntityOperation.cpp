#include "CopyEntityOperation.h"

#include "Editor.h"
#include "game/SceneGraph.hpp"

using namespace engine::game;
using namespace engine::editor;

CCopyEntityOperation::CCopyEntityOperation(entt::entity parent, entt::entity target)
{
	this->parent = parent;
	this->target = target;
	redo();
}

void CCopyEntityOperation::undo()
{
	scenegraph::destroy_node(entity);
	EGEditor->deselectAll();
}

void CCopyEntityOperation::redo()
{
	entity = scenegraph::duplicate_node(target);
	scenegraph::attach_child(parent, entity);
	EGEditor->selectObject(entity);
}