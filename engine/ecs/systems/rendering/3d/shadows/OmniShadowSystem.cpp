#include "OmniShadowSystem.h"

#include "Engine.h"

#include "ecs/components/components.h"

using namespace engine::graphics;
using namespace engine::ecs;

void COmniShadowSystem::__create()
{
	FShaderSpecials specials;
	specials.usages = MAX_POINT_LIGHT_SHADOW_COUNT * 6;
	shader_id = graphics->addShader("omni_shadow_pass", specials);

	CBaseGraphicsSystem::__create();
}

void COmniShadowSystem::__update(float fDt)
{
	auto& shadowManager = EGEngine->getShadows();

	const auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	uint32_t point_light_count{ 0 };
	std::array<glm::mat4, 6> point_light_view_matrices;
	std::array<FFrustum, 6> point_light_frustums;

	auto stage = graphics->getRenderStageID("omni_shadow");
	graphics->bindRenderer(stage);

	// Calculating point light matrices
	{
		auto lights = registry->view<FTransformComponent, FPointLightComponent>();
		for (auto [entity, ltransform, light] : lights.each())
		{
			light.shadowIndex = -1;

			// Skip shadow calculating if light is not casting shadows and if current light count is less them max shadow count
			if (!light.castShadows && point_light_count < MAX_POINT_LIGHT_SHADOW_COUNT)
				continue;

			auto& shadow_commit = shadowManager->getPointShadowCommit(point_light_count);

			uint32_t array_shift = point_light_count * 6;

			glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, light.radius);
			point_light_view_matrices[0] = glm::lookAt(ltransform.rposition, ltransform.rposition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)); // POSITIVE_X
			point_light_view_matrices[1] = glm::lookAt(ltransform.rposition, ltransform.rposition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)); // NEGATIVE_X
			point_light_view_matrices[2] = glm::lookAt(ltransform.rposition, ltransform.rposition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)); // POSITIVE_Y
			point_light_view_matrices[3] = glm::lookAt(ltransform.rposition, ltransform.rposition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));	// NEGATIVE_Y
			point_light_view_matrices[4] = glm::lookAt(ltransform.rposition, ltransform.rposition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));	// POSITIVE_Z
			point_light_view_matrices[5] = glm::lookAt(ltransform.rposition, ltransform.rposition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));	// NEGATIVE_Z

			auto distance = glm::distance(camera->viewPos, ltransform.rposition);
			auto lod_level = getLodLevel(camera->nearPlane, camera->farPlane, distance);

			graphics->bindShader(shader_id);
			graphics->setManualShaderControlFlag(true);

			for (uint32_t i = 0; i < 6; i++)
			{
				auto shadowViewProj = shadowProj * point_light_view_matrices[i];
				point_light_frustums[i].update(point_light_view_matrices[i], shadowProj);

				auto& pPush = graphics->getPushBlockHandle("modelData");
				pPush->set("viewProjMat", shadowViewProj);
				pPush->set("position", glm::vec4(ltransform.rposition, 1.f));
				pPush->set("farPlane", light.radius);
				pPush->set("stride", array_shift + i);

				auto meshes = registry->view<FTransformComponent, FMeshComponent>();
				for (auto [entity, mtransform, mesh] : meshes.each())
				{
					if (!graphics->bindVertexBuffer(mesh.vbo_id))
						continue;

					pPush->set("model", mtransform.model);
					graphics->flushConstantRanges(pPush);

					auto& head = registry->get<FSceneComponent>(mesh.head);

					for (auto& meshlet : mesh.vMeshlets)
					{
						auto inLightView = head.castShadows && point_light_frustums[i].checkBox(mtransform.rposition + meshlet.dimensions.min * mtransform.rscale, mtransform.rposition + meshlet.dimensions.max * mtransform.rscale);

						if (inLightView)
						{
							auto& lod = meshlet.levels_of_detail[lod_level];
							graphics->draw(lod.begin_vertex, lod.vertex_count, lod.begin_index, lod.index_count);
						}
					}

					graphics->bindVertexBuffer(invalid_index);
				}
			}

			graphics->setManualShaderControlFlag(false);
			graphics->bindShader(invalid_index);

			// Set shadow index and increment it
			shadow_commit.farPlane = light.radius;
			shadow_commit.index = light.shadowIndex = point_light_count;
			point_light_count++;
		}
	}

	graphics->bindRenderer(invalid_index);
}