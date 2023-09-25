#include "HierarchySystem.h"

#include "Engine.h"

#include "ecs/components/components.h"

#include <glm/gtx/quaternion.hpp>

using namespace engine::ecs;

void CHierarchySystem::__create()
{

}

void CHierarchySystem::__update(float fDt)
{
	fDt;
	auto root = EGSceneManager->getRoot();

	if (!registry->valid(root))
		return;

	build_hierarchy(registry, root);
}

void CHierarchySystem::build_hierarchy(entt::registry* registry, entt::entity node)
{
	auto& transform = registry->get<FTransformComponent>(node);
	auto& hierarchy = registry->get<FHierarchyComponent>(node);

	transform.model_old = transform.model;
	initialize_matrix(&transform);

	if (registry->valid(hierarchy.parent)) 
	{
		auto parentTransform = registry->get<FTransformComponent>(hierarchy.parent);
		transform.model = parentTransform.model * transform.model;
	}

	transform.update();

	for (auto& child : hierarchy.children)
		build_hierarchy(registry, child);
}

void CHierarchySystem::initialize_matrix(FTransformComponent* transform)
{
	transform->model = glm::translate(glm::mat4(1.0f), transform->position);
	transform->model *= glm::mat4_cast(glm::normalize(transform->rotation));
	transform->model = glm::scale(transform->model, transform->scale);
	transform->model *= transform->matrix;
	transform->update();
}

// Only for children
void CHierarchySystem::initialize_matrices(entt::registry* registry, entt::entity node)
{
	std::queue<entt::entity> nextparent;
	nextparent.push(node);

	while (!nextparent.empty())
	{
		auto current = nextparent.front();

		auto& hierarchy = registry->get<FHierarchyComponent>(current);
		for (auto& child : hierarchy.children)
		{
			auto& ctransform = registry->get<FTransformComponent>(child);
			auto& chierarchy = registry->get<FHierarchyComponent>(child);

			initialize_matrix(&ctransform);

			if (!chierarchy.children.empty())
				nextparent.push(child);
		}
		nextparent.pop();
	}
}

void CHierarchySystem::calculate_matrices(entt::registry* registry, entt::entity node)
{
	std::queue<entt::entity> nextparent;
	nextparent.push(node);

	while (!nextparent.empty())
	{
		auto current = nextparent.front();

		auto& transform = registry->get<FTransformComponent>(current);
		auto& hierarchy = registry->get<FHierarchyComponent>(current);
		transform.update();

		if (hierarchy.name == "Skeleton_torso_joint_1")
		{
			int i = 0;
			i += 10;
		}

		if (transform.model != transform.model_old)
			transform.normal = glm::transpose(glm::inverse(transform.model));

		for (auto& child : hierarchy.children)
		{
			auto& ctransform = registry->get<FTransformComponent>(child);
			auto& chierarchy = registry->get<FHierarchyComponent>(child);

			ctransform.model = transform.model * ctransform.model;
			ctransform.update();

			// Add all children to the queue, not only the ones that have their own children
			nextparent.push(child);
		}
		nextparent.pop();
	}
}