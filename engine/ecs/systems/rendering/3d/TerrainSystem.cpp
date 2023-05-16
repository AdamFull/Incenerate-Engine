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

	graphics->bindCameraData(camera->view, camera->projection, camera->frustum.getFrustumSides());

	auto view = registry->view<FTransformComponent, FTerrainComponent>();
	for (auto [entity, transform, terrain] : view.each())
	{
		if (!graphics->bindVertexBuffer(terrain.vbo_id))
			continue;

		graphics->bindObjectData(transform.model, transform.normal, static_cast<uint32_t>(entity));

		bool needToRender{ true };
		//needToRender = camera->frustum.checkBox(transform.rposition + terrain.min * transform.rscale, transform.rposition + terrain.max * transform.rscale);
		
		if (needToRender)
		{
			graphics->bindMaterial(terrain.material_id);
			graphics->draw();
		}

		//graphics->bindMaterial(invalid_index);
		graphics->bindVertexBuffer(invalid_index);
	}
}