#include "DebugDrawSystem.h"

#include "Engine.h"
#include "ecs/components/components.h"

using namespace engine::ecs;

void CDebugDrawSystem::__create()
{
	shader_id = graphics->addShader("displaynormal", "displaynormal");
}

void CDebugDrawSystem::__update(float fDt)
{
	auto& debug_draw = graphics->getDebugDraw();

	auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	if (EGEngine->isDebugDrawNormals())
	{
		graphics->bindShader(shader_id);
		graphics->setManualShaderControlFlag(true);
		graphics->flushShader();

		auto view = registry->view<FTransformComponent, FMeshComponent>();
		for (auto [entity, transform, mesh] : view.each())
		{
			graphics->bindVertexBuffer(mesh.vbo_id);

			for (auto& meshlet : mesh.vMeshlets)
			{
				bool needToRender = camera->frustum.checkBox(transform.rposition + meshlet.dimensions.min * transform.rscale, transform.rposition + meshlet.dimensions.max * transform.rscale);
				if (needToRender)
				{
					auto& pPush = graphics->getPushBlockHandle("debug");
					pPush->set("projection", camera->projection);
					pPush->set("view", camera->view);
					pPush->set("model", transform.model);
					pPush->set("normal", transform.normal);
					graphics->flushConstantRanges(pPush);

					graphics->draw(meshlet.begin_vertex, meshlet.vertex_count, meshlet.begin_index, meshlet.index_count);
				}
			}

			graphics->bindVertexBuffer(invalid_index);
		}

		graphics->setManualShaderControlFlag(false);
		graphics->bindShader(invalid_index);
	}

	debug_draw->draw();
}