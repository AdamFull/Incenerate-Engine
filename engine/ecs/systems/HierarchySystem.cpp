#include "HierarchySystem.h"

#include "Engine.h"

#include "ecs/components/components.h"

using namespace engine::ecs;

void CHierarchySystem::__create()
{

}

void CHierarchySystem::__update(float fDt)
{
	fDt;
	auto root = EGSceneManager->getRoot();

	// Preparing transformation
	auto view = registry->view<FTransformComponent>();
	for (auto [entity, transform] : view.each())
	{
		transform.model_old = transform.model;
		initialize_matrix(&transform);
	}

	if (root != entt::null)
		calculate_matrices(registry, root);
}

void CHierarchySystem::initialize_matrix(FTransformComponent* transform)
{
	transform->model = glm::mat4(1.f);

	transform->model = glm::translate(transform->model, transform->position);
	if (transform->rotation.x != 0.f)
		transform->model = glm::rotate(transform->model, transform->rotation.x, glm::vec3(1.f, 0.f, 0.f));
	if (transform->rotation.y != 0.f)
		transform->model = glm::rotate(transform->model, transform->rotation.y, glm::vec3(0.f, 1.f, 0.f));
	if (transform->rotation.z != 0.f)
		transform->model = glm::rotate(transform->model, transform->rotation.z, glm::vec3(0.f, 0.f, 1.f));
	transform->model = glm::scale(transform->model, transform->scale);
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
		transform.normal = glm::transpose(glm::inverse(transform.model));

		for (auto& child : hierarchy.children)
		{
			auto& ctransform = registry->get<FTransformComponent>(child);
			auto& chierarchy = registry->get<FHierarchyComponent>(child);

			ctransform.model = transform.model * ctransform.model;
			ctransform.update();

			if (!chierarchy.children.empty())
				nextparent.push(child);
			else
				ctransform.normal = glm::transpose(glm::inverse(ctransform.model));
		}
		nextparent.pop();
	}
}