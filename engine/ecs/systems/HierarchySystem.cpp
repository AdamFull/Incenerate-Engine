#include "HierarchySystem.h"

#include "Engine.h"

#include "ecs/components/components.h"

using namespace engine::ecs;

void CHierarchySystem::__create()
{

}

void CHierarchySystem::__update(float fDt)
{
	auto& registry = EGCoordinator;
	auto root = EGSceneManager->getRoot();

	// Preparing transformation
	auto view = registry.view<FTransformComponent>();
	for (auto [entity, transform] : view.each())
	{
		transform.model = glm::mat4(1.f);

		transform.model = glm::translate(transform.model, transform.position);
		if (transform.rotation.x != 0)
			transform.model = glm::rotate(transform.model, transform.rotation.x, glm::vec3(1.0, 0.0, 0.0));
		if (transform.rotation.y != 0)
			transform.model = glm::rotate(transform.model, transform.rotation.y, glm::vec3(0.0, 1.0, 0.0));
		if (transform.rotation.z != 0)
			transform.model = glm::rotate(transform.model, transform.rotation.z, glm::vec3(0.0, 0.0, 1.0));
		transform.model = glm::scale(transform.model, transform.scale);
		transform.update();
	}

	if (root != entt::null)
	{
		// Calculating relative transform
		std::queue<entt::entity> nextparent;
		nextparent.push(root);

		while (!nextparent.empty())
		{
			auto current = nextparent.front();

			auto& transform = registry.get<FTransformComponent>(current);
			auto& hierarchy = registry.get<FHierarchyComponent>(current);
			transform.update();

			for (auto& child : hierarchy.children)
			{
				auto& ctransform = registry.get<FTransformComponent>(child);
				auto& chierarchy = registry.get<FHierarchyComponent>(child);

				ctransform.model = transform.model * ctransform.model;
				ctransform.update();

				if (!chierarchy.children.empty())
					nextparent.push(child);
			}
			nextparent.pop();
		}
	}
}