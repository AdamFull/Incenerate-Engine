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

	auto view = registry->view<FTransformComponent, FMeshComponent>();
	for (auto [entity, transform, mesh] : view.each())
	{
		if (!graphics->bindVertexBuffer(mesh.vbo_id))
			continue;

		bool bHasSkin{ mesh.skin > -1 };
		auto distance = glm::distance(camera->viewPos, transform.rposition);
		auto lod_level = getLodLevel(camera->nearPlane, camera->farPlane, distance);

		for (auto& meshlet : mesh.vMeshlets)
		{
			bool needToRender{ true };
			//needToRender = camera->frustum.checkBox(transform.rposition + meshlet.dimensions.min * transform.rscale, transform.rposition + meshlet.dimensions.max * transform.rscale);
			meshlet.bWasCulled = needToRender;
		
			needToRender = needToRender && graphics->compareAlphaMode(meshlet.material, alphaMode);
		
			if (needToRender)
			{
				if (graphics->bindMaterial(meshlet.material))
				{
					auto& pInstanceUBO = graphics->getUniformHandle("UBOInstancing");
					if (pInstanceUBO)
					{
						pInstanceUBO->set("instances", mesh.vInstances);
					}

					auto& pJoints = graphics->getUniformHandle("FSkinning");
					if (pJoints && bHasSkin)
					{
						auto& scene = registry->get<FSceneComponent>(mesh.head);
						auto& skin = scene.skins[mesh.skin];
						pJoints->set("jointMatrices", skin.jointMatrices);
					}

					graphics->bindObjectData(transform.model, transform.normal, static_cast<uint32_t>(entity));
		
					graphics->flushShader();
				}
		
				auto& lod = meshlet.levels_of_detail[lod_level];
				debug_draw->drawDebugAABB(transform.rposition + meshlet.dimensions.min * transform.rscale, transform.rposition + meshlet.dimensions.max * transform.rscale);
					
				graphics->draw(lod.begin_vertex, lod.vertex_count, lod.begin_index, lod.index_count, mesh.instanceCount == 0 ? 1 : mesh.instanceCount);
			}
		
			//graphics->bindMaterial(invalid_index);
		}
	}

	graphics->bindVertexBuffer(invalid_index);
}