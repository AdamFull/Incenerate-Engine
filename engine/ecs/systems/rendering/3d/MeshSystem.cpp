#include "MeshSystem.h"


#include "Engine.h"

#include "ecs/components/components.h"
#include "ecs/helper.hpp"
#include <Helpers.h>

using namespace engine::graphics;
using namespace engine::ecs;
//map <mat_id, draw_data>

void CMeshSystem::__create()
{
	CBaseGraphicsSystem::__create();
}

void CMeshSystem::__update(float fDt)
{
	auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	graphics->bindCameraData(camera->view, camera->projection, camera->frustum.getFrustumSides());

	draw(camera, EAlphaMode::EOPAQUE);
	draw(camera, EAlphaMode::EMASK);
	draw(camera, EAlphaMode::EBLEND);

	graphics->bindMaterial(invalid_index);
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

		auto distance = glm::distance(camera->viewPos, transform.rposition);
		auto lod_level = getLodLevel(camera->nearPlane, camera->farPlane, distance);

		for (auto& meshlet : mesh.vMeshlets)
		{
			bool needToRender{ true };
			needToRender = camera->frustum.checkBox(transform.rposition + meshlet.dimensions.min * transform.rscale, transform.rposition + meshlet.dimensions.max * transform.rscale);
			meshlet.bWasCulled = needToRender;

			needToRender = needToRender && graphics->compareAlphaMode(meshlet.material, alphaMode);

			if (needToRender)
			{
				graphics->bindMaterial(meshlet.material);
				auto& lod = meshlet.levels_of_detail[lod_level];
				//debug_draw->drawDebugAABB(transform.rposition + meshlet.dimensions.min * transform.rscale, transform.rposition + meshlet.dimensions.max * transform.rscale);

				auto& pJoints = graphics->getUniformHandle("FSkinning");
				if (pJoints && bHasSkin)
					pJoints->set("jointMatrices", joints);

				graphics->bindObjectData(transform.model, transform.normal, static_cast<uint32_t>(entity));
					
				graphics->draw(lod.begin_vertex, lod.vertex_count, lod.begin_index, lod.index_count);
			}

			//graphics->bindMaterial(invalid_index);
		}
	}

	graphics->bindVertexBuffer(invalid_index);
}