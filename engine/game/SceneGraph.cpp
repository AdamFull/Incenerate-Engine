#include "SceneGraph.hpp"

#include "Engine.h"
#include "ecs/components/TransformComponent.h"

using namespace engine::ecs;
using namespace engine::game;

CSceneNode::CSceneNode()
{
	entity = EGCoordinator.create();
	EGCoordinator.emplace<FTransformComponent>(entity, FTransformComponent{});
	log_debug("Entity with id {} was created", static_cast<uint32_t>(entity));
}

CSceneNode::CSceneNode(const std::string& name)
{
	srName = name;
	entity = EGCoordinator.create();
	EGCoordinator.emplace<FTransformComponent>(entity, FTransformComponent{});
	log_debug("Entity with id {} and name {} was created", static_cast<uint32_t>(entity), name);
}

CSceneNode::~CSceneNode()
{
	vChildren.clear();
	EGCoordinator.destroy(entity);
	pParent = nullptr;
	log_debug("Entity with id {} was destroyed", static_cast<uint32_t>(entity));
}

std::stack<entt::entity> CSceneNode::relative(entt::entity id)
{
	std::stack<entt::entity> relatives;
	CSceneNode* pNode = find(id, true).get();
	while (pNode)
	{
		relatives.emplace(pNode->entity);
		pNode = pNode->pParent;
	}

	return relatives;
}

void CSceneNode::exchange(CSceneNode* other, entt::entity id)
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

std::unique_ptr<CSceneNode> CSceneNode::detach(entt::entity id)
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

const std::unique_ptr<CSceneNode>& CSceneNode::find(entt::entity id, bool bDeep) const
{
	return *__find([&id](const std::unique_ptr<CSceneNode>& node) { return id == node->getEntity(); }, bDeep);
}

const std::unique_ptr<CSceneNode>& CSceneNode::find(const std::string& name, bool bDeep) const
{
	return *__find([&name](const std::unique_ptr<CSceneNode>& node) { return name == node->getName(); }, bDeep);
}

const bool CSceneNode::isVisible(entt::entity id) const
{
	// Wrong
	auto& found = find(id);
	return found ? found->isVisible() : false;
}

const bool CSceneNode::isEnabled(entt::entity id) const
{
	// Wrong
	auto& found = find(id);
	return found ? found->isEnabled() : false;
}