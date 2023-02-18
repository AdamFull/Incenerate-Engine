#include "TerrainSystem.h"

#include "Engine.h"

#include "ecs/components/TerrainComponent.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/CameraComponent.h"

using namespace engine::ecs;

void CTerrainSystem::__create()
{
	CBaseGraphicsSystem::__create();
}

void CTerrainSystem::__update(float fDt)
{
	auto& device = graphics->getDevice();
	auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	auto view = registry->view<FTransformComponent, FTerrainComponent>();
	for (auto [entity, transform, terrain] : view.each())
	{
		if (!graphics->bindVertexBuffer(terrain.vbo_id))
			continue;

		bool needToRender{ true };
		//needToRender = camera->frustum.checkBox(transform.rposition + terrain.min * transform.rscale, transform.rposition + terrain.max * transform.rscale);
		
		graphics->bindMaterial(terrain.material_id);

		if (needToRender)
		{
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

			graphics->draw();
		}

		graphics->bindMaterial(invalid_index);
		graphics->bindVertexBuffer(invalid_index);
	}
}