#include "MeshSystem.h"


#include "Engine.h"

#include "ecs/components/components.h"
#include "ecs/helper.hpp"
#include <Helpers.h>

using namespace engine::graphics;
using namespace engine::ecs;

void CMeshSystem::__create()
{
	CBaseGraphicsSystem::__create();
}

void CMeshSystem::__update(float fDt)
{
	auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	draw(camera, EAlphaMode::EOPAQUE);
	draw(camera, EAlphaMode::EMASK);
	draw(camera, EAlphaMode::EBLEND);
}

void CMeshSystem::draw(const FCameraComponent* camera, EAlphaMode alphaMode)
{
	auto& device = graphics->getDevice();
	auto& debug_draw = graphics->getDebugDraw();

	static std::array<glm::mat4, 128> joints{glm::mat4(1.f)};

	auto view = registry->view<FTransformComponent, FMeshComponent>();
	for (auto [entity, transform, mesh] : view.each())
	{
		if (!graphics->bindVertexBuffer(mesh.vbo_id))
			continue;

		bool bHasSkin{ false };
		entt::entity armature{ entt::null };
		if (mesh.skin > -1)
		{
			bHasSkin = true;
			auto& scene = registry->get<FSceneComponent>(mesh.head);
			auto invTransform = glm::inverse(transform.model);
			auto& skin = scene.skins[mesh.skin];
			armature = skin.root;

			for (uint32_t i = 0; i < skin.joints.size(); i++)
			{
				auto& jtransform = registry->get<FTransformComponent>(skin.joints[i]);
				joints[i] = jtransform.model * skin.inverseBindMatrices[i];
				joints[i] = invTransform * joints[i];
			}
		}

		for (auto& meshlet : mesh.vMeshlets)
		{
			bool needToRender{ true };
			needToRender = camera->frustum.checkBox(transform.rposition + meshlet.dimensions.min * transform.rscale, transform.rposition + meshlet.dimensions.max * transform.rscale);
			meshlet.bWasCulled = needToRender;

			graphics->bindMaterial(meshlet.material);

			needToRender = needToRender && graphics->compareAlphaMode(alphaMode);

			if (needToRender)
			{
				//debug_draw->drawDebugAABB(transform.rposition + meshlet.dimensions.min * transform.rscale, transform.rposition + meshlet.dimensions.max * transform.rscale);
				auto index = static_cast<uint32_t>(entity);

				auto& pUBO = graphics->getUniformHandle("FUniformData");
				pUBO->set("model", transform.model);
				pUBO->set("view", camera->view);
				pUBO->set("projection", camera->projection);
				pUBO->set("normal", transform.normal);
				pUBO->set("viewDir", camera->viewPos);
				pUBO->set("viewportDim", device->getExtent(true));
				pUBO->set("frustumPlanes", camera->frustum.getFrustumSides());
				pUBO->set("object_id", encodeIdToColor(index));

				auto& pJoints = graphics->getUniformHandle("FSkinning");
				if (pJoints && bHasSkin)
					pJoints->set("jointMatrices", joints);
					
				//graphics->beginQuery(index);
				graphics->draw(meshlet.begin_vertex, meshlet.vertex_count, meshlet.begin_index, meshlet.index_count);
				//graphics->endQuery(index);
			}

			//graphics->bindMaterial(invalid_index);
		}

		graphics->bindVertexBuffer(invalid_index);
	}
}