#include "ExchangeEntityOperation.h"

#include "Engine.h"

#include "game/SceneGraph.hpp"
#include "ecs/components/HierarchyComponent.h"

using namespace engine::ecs;
using namespace engine::game;
using namespace engine::editor;

CExchangeEntityOperation::CExchangeEntityOperation(entt::entity target, entt::entity new_parent)
{
	this->target = target;
	this->new_parent = new_parent;
	redo();
}

void CExchangeEntityOperation::undo()
{
	redo();
}

void CExchangeEntityOperation::redo()
{
	auto& registry = EGEngine->getRegistry();
	auto& hierarchy = registry.get<FHierarchyComponent>(target);
	auto old_parent = hierarchy.parent;

	scenegraph::parent_exchange(new_parent, target);
	new_parent = old_parent;
}