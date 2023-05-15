#include "OmniShadowSystem.h"

#include "Engine.h"

#include "ecs/components/components.h"

using namespace engine::graphics;
using namespace engine::ecs;

void COmniShadowSystem::__create()
{
	FShaderSpecials specials;
	specials.usages = MAX_POINT_LIGHT_COUNT * 6;
	shader_id = graphics->addShader("omni_shadow_pass", specials);

	CBaseGraphicsSystem::__create();
}

void COmniShadowSystem::__update(float fDt)
{
	uint32_t point_light_count{ 0 };
	std::array<glm::mat4, 6> point_light_view_matrices;
	std::array<FFrustum, 6> point_light_frustums;

	auto stage = graphics->getRenderStageID("omni_shadow");
	graphics->bindRenderer(stage);

	// Calculating point light matrices
	{
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 32.f);

		auto lights = registry->view<FTransformComponent, FPointLightComponent>();
		for (auto [entity, ltransform, light] : lights.each())
		{
			if (!light.castShadows)
				continue;

			uint32_t array_shift = point_light_count * 6;

			point_light_view_matrices[0] = glm::lookAt(ltransform.rposition, ltransform.rposition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)); // POSITIVE_X
			point_light_view_matrices[1] = glm::lookAt(ltransform.rposition, ltransform.rposition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)); // NEGATIVE_X
			point_light_view_matrices[2] = glm::lookAt(ltransform.rposition, ltransform.rposition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)); // POSITIVE_Y
			point_light_view_matrices[3] = glm::lookAt(ltransform.rposition, ltransform.rposition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));	// NEGATIVE_Y
			point_light_view_matrices[4] = glm::lookAt(ltransform.rposition, ltransform.rposition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));	// POSITIVE_Z
			point_light_view_matrices[5] = glm::lookAt(ltransform.rposition, ltransform.rposition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));	// NEGATIVE_Z

			graphics->bindShader(shader_id);
			graphics->setManualShaderControlFlag(true);

			for (uint32_t i = 0; i < 6; i++)
			{
				point_light_frustums[i].update(point_light_view_matrices[i], shadowProj);

				auto& pUniform = graphics->getUniformHandle("UBOShadowmap");
				pUniform->set("viewProjMat", shadowProj * point_light_view_matrices[i]);
				pUniform->set("position", glm::vec4(ltransform.rposition, 1.f));
				pUniform->set("farPlane", light.radius);
				graphics->flushShader();

				auto meshes = registry->view<FTransformComponent, FMeshComponent>();
				for (auto [entity, mtransform, mesh] : meshes.each())
				{
					if (!graphics->bindVertexBuffer(mesh.vbo_id))
						continue;

					auto& head = registry->get<FSceneComponent>(mesh.head);

					for (auto& meshlet : mesh.vMeshlets)
					{
						auto inLightView = head.castShadows && point_light_frustums[i].checkBox(mtransform.rposition + meshlet.dimensions.min * mtransform.rscale, mtransform.rposition + meshlet.dimensions.max * mtransform.rscale);

						if (inLightView)
						{
							auto& pPush = graphics->getPushBlockHandle("modelData");
							pPush->set("model", mtransform.model);
							pPush->set("stride", array_shift + i);
							graphics->flushConstantRanges(pPush);

							graphics->draw(meshlet.begin_vertex, meshlet.vertex_count, meshlet.begin_index, meshlet.index_count);
						}
					}

					graphics->bindVertexBuffer(invalid_index);
				}
			}

			graphics->setManualShaderControlFlag(false);
			graphics->bindShader(invalid_index);

			point_light_count++;
		}
	}

	graphics->bindRenderer(invalid_index);
}