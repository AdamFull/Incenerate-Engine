#include "DebugDrawSystem.h"

#include "Engine.h"
#include "ecs/components/components.h"

#include <SessionStorage.hpp>

using namespace engine::ecs;
using namespace engine::graphics;

#define DEBUG_DRAW_MAX_VERTICES 16384

void CDebugDrawSystem::__create()
{
	normals_shader_id = graphics->addShader("displaynormal");

	debug_vbo_id = graphics->addVertexBuffer("debug_draw_vbo");

	auto& vbo = graphics->getVertexBuffer(debug_vbo_id);
	vbo->reserve(sizeof(FSimpleVertex), DEBUG_DRAW_MAX_VERTICES, sizeof(uint32_t), 0);

	debug_shader_id = graphics->addShader("debugdraw");
}

void CDebugDrawSystem::__update(float fDt)
{
	auto& debug_draw = graphics->getDebugDraw();

	auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	if (CSessionStorage::getInstance()->get<bool>("display_normals"))
	{
		graphics->bindShader(normals_shader_id);
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

	debug_draw->drawDebugGrid(-100.f, 100.f, 0.f, 1.f);
	debug_draw->drawDebugCross(glm::vec3(0.f), 3.f);

	auto& pVBO = graphics->getVertexBuffer(debug_vbo_id);

	auto& draw_list = debug_draw->getDrawList();
	if (!draw_list.empty())
	{
		pVBO->clear();
		pVBO->addVertices(draw_list);
		pVBO->setLoaded();

		graphics->bindShader(debug_shader_id);
		if (!graphics->bindVertexBuffer(debug_vbo_id))
		{
			graphics->bindShader(invalid_index);
			return;
		}

		auto& pUniform = graphics->getUniformHandle("UBODebugDraw");
		pUniform->set("projection", camera->projection);
		pUniform->set("view", camera->view);

		graphics->flushShader();
		graphics->draw(0, pVBO->getLastVertex(), 0, 0, 1);

		graphics->bindVertexBuffer(invalid_index);
		graphics->bindShader(invalid_index);

		debug_draw->clear();
	}
}