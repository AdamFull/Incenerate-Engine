#include "SkinningSystem.h"

#include "Engine.h"

#include "ecs/components/components.h"

using namespace engine::ecs;

void CSkinningSystem::__create()
{

}

void CSkinningSystem::__update(float fDt)
{
	auto view = registry->view<FTransformComponent, FMeshComponent>();
	for (auto [entity, transform, mesh] : view.each())
	{
		if (mesh.skin > -1)
		{
			auto& scene = registry->get<FSceneComponent>(mesh.head);
			auto invTransform = glm::inverse(transform.model);
			auto& skin = scene.skins[mesh.skin];
			
			debugDrawSkeleton(skin.root);

			for (uint32_t i = 0; i < skin.joints.size(); i++)
			{
				auto& jtransform = registry->get<FTransformComponent>(skin.joints[i]);
				skin.jointMatrices[i] = jtransform.model * skin.inverseBindMatrices[i];
				skin.jointMatrices[i] = invTransform * skin.jointMatrices[i];
			}
		}
	}
}

void CSkinningSystem::debugDrawSkeleton(entt::entity& node, glm::mat4 parentMatrix)
{
	auto& graphics = EGEngine->getGraphics();
	auto& debug_draw = graphics->getDebugDraw();

	auto& nodeTransform = registry->get<FTransformComponent>(node);
	auto& nodeHierarchy = registry->get<FHierarchyComponent>(node);

	glm::vec3 nodePosition = glm::vec3(nodeTransform.model[3]);

	if (parentMatrix != glm::mat4(1.0f))
	{
		glm::vec3 parentPosition = glm::vec3(parentMatrix[3]);
		debug_draw->drawDebugLine(parentPosition, nodePosition);
	}

	for (auto child : nodeHierarchy.children)
		debugDrawSkeleton(child, nodeTransform.model);
}