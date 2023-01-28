#include "MeshSystem.h"


#include "Engine.h"

#include "ecs/components/components.h"
#include "ecs/helper.hpp"

using namespace engine::graphics;
using namespace engine::ecs;

void CMeshSystem::__create()
{
	CBaseGraphicsSystem::__create();
}

void CMeshSystem::__update(float fDt)
{
	auto& registry = EGCoordinator;
	auto editorMode = EGEngine->isEditorMode();
	auto state = EGEngine->getState();

	FCameraComponent* camera{ nullptr };

	if (editorMode && state == EEngineState::eEditing)
		camera = registry.try_get<FCameraComponent>(EGEditor->getCamera());
	else
		camera = registry.try_get<FCameraComponent>(get_active_camera(registry));

	if (!camera)
		return;

	draw(camera, EAlphaMode::EOPAQUE);
	draw(camera, EAlphaMode::EMASK);
	draw(camera, EAlphaMode::EBLEND);
}

void CMeshSystem::draw(const FCameraComponent* camera, EAlphaMode alphaMode)
{
	auto& registry = EGCoordinator;
	auto& graphics = EGGraphics;
	
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
				auto normal = glm::transpose(glm::inverse(transform.model));

				auto& pUBO = graphics->getUniformHandle("FUniformData");
				pUBO->set("model", transform.model);
				pUBO->set("view", camera->view);
				pUBO->set("projection", camera->projection);
				pUBO->set("normal", normal);
				pUBO->set("viewDir", camera->viewPos);
				pUBO->set("viewportDim", EGGraphics->getDevice()->getExtent(true));
				pUBO->set("frustumPlanes", camera->frustum.getFrustumSides());
				pUBO->set("object_id", VkHelper::idToColor(static_cast<uint32_t>(entity)));

				graphics->draw(meshlet.begin_vertex, meshlet.vertex_count, meshlet.begin_index, meshlet.index_count);
			}

			graphics->bindMaterial(invalid_index);
		}

		graphics->bindVertexBuffer(invalid_index);
	}
}