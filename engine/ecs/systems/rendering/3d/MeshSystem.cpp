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
	auto& registry = EGEngine->getRegistry();

	auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	draw(camera, EAlphaMode::EOPAQUE);
	draw(camera, EAlphaMode::EMASK);
	draw(camera, EAlphaMode::EBLEND);
}

void CMeshSystem::draw(const FCameraComponent* camera, EAlphaMode alphaMode)
{
	auto& registry = EGEngine->getRegistry();
	auto& device = graphics->getDevice();
	auto& debug_draw = graphics->getDebugDraw();
	
	auto view = registry.view<FTransformComponent, FMeshComponent>();
	for (auto [entity, transform, mesh] : view.each())
	{
		graphics->bindVertexBuffer(mesh.vbo_id);

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

				auto& pUBO = graphics->getUniformHandle("FUniformData");
				pUBO->set("model", transform.model);
				pUBO->set("view", camera->view);
				pUBO->set("projection", camera->projection);
				pUBO->set("normal", transform.normal);
				pUBO->set("viewDir", camera->viewPos);
				pUBO->set("viewportDim", device->getExtent(true));
				pUBO->set("frustumPlanes", camera->frustum.getFrustumSides());
				pUBO->set("object_id", encodeIdToColor(static_cast<uint32_t>(entity)));



				graphics->draw(meshlet.begin_vertex, meshlet.vertex_count, meshlet.begin_index, meshlet.index_count);
			}

			graphics->bindMaterial(invalid_index);
		}

		graphics->bindVertexBuffer(invalid_index);
	}
}