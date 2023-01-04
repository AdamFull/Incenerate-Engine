#include "SceneGraph.hpp"

#include "Engine.h"
#include "components/TransformComponent.h"

using namespace engine::ecs;

CSceneNode::CSceneNode()
{
	entity = EGCoordinator->createEntity();
	EGCoordinator->addComponent(entity, FTransformComponent{});
	log_debug("Entity with id {} was created", entity);
}

CSceneNode::~CSceneNode()
{
	vChildren.clear();
	EGCoordinator->destroyEntity(entity);
	pParent = nullptr;
	log_debug("Entity with id {} was destroyed", entity);
}

std::stack<Entity> CSceneNode::relative(Entity id)
{
	std::stack<Entity> relatives;
	CSceneNode* pNode = find(id, true).get();
	while (pNode)
	{
		relatives.emplace(pNode->entity);
		pNode = pNode->pParent;
	}

	return relatives;
}

void CSceneNode::exchange(CSceneNode* other, Entity id)
{
	attach(other->detach(id));
}

void CSceneNode::exchange(CSceneNode* other, const std::string& name)
{
	attach(other->detach(name));
}

void CSceneNode::exchange(CSceneNode* other, CSceneNode* child)
{
	attach(
		other->detach(
			__find(
				[&child](const std::unique_ptr<CSceneNode>& node) { 
					return child->getEntity() == node->getEntity(); 
				}
			)
		)
	);
}

void CSceneNode::attach(std::unique_ptr<CSceneNode>&& node)
{
	vChildren.emplace_back(std::move(node));
	vChildren.back()->pParent = this;
}

std::unique_ptr<CSceneNode> CSceneNode::detach(const std::string& name)
{
	return detach(
		__find(
			[&name](const std::unique_ptr<CSceneNode>& node) {
				return name == node->getName();
			}
		)
	);
}

std::unique_ptr<CSceneNode> CSceneNode::detach(Entity id)
{
	return detach(
		__find(
			[&id](const std::unique_ptr<CSceneNode>& node) {
				return id == node->getEntity();
			}
		)
	);
}

std::unique_ptr<CSceneNode> CSceneNode::detach(child_container_t::const_iterator node)
{
	std::unique_ptr<CSceneNode> pDetached;

	if (node != vChildren.cend())
	{
		pDetached = std::move(const_cast<std::unique_ptr<CSceneNode>&>((*node)));
		vChildren.erase(node);
	}

	return pDetached;
}

const std::unique_ptr<CSceneNode>& CSceneNode::find(Entity id, bool bDeep) const
{
	return *__find([&id](const std::unique_ptr<CSceneNode>& node) { return id == node->getEntity(); }, bDeep);
}

const std::unique_ptr<CSceneNode>& CSceneNode::find(const std::string& name, bool bDeep) const
{
	return *__find([&name](const std::unique_ptr<CSceneNode>& node) { return name == node->getName(); }, bDeep);
}

const bool CSceneNode::isVisible(Entity id) const
{
	// Wrong
	auto& found = find(id);
	return found ? found->isVisible() : false;
}

const bool CSceneNode::isEnabled(Entity id) const
{
	// Wrong
	auto& found = find(id);
	return found ? found->isEnabled() : false;
}