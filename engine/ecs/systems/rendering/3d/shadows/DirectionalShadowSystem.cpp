#include "DirectionalShadowSystem.h"

#include "Engine.h"

#include "ecs/components/components.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CDirectionalShadowSystem::__create()
{
	FShaderSpecials specials;
	specials.usages = MAX_SPOT_LIGHT_COUNT;
	//specials.defines = { {"INVOCATION_COUNT", std::to_string(MAX_SPOT_LIGHT_COUNT)} };
	shader_id = graphics->addShader("directional_shadow_pass", "directional_shadow_pass", specials);

	CBaseGraphicsSystem::__create();
}

void CDirectionalShadowSystem::__update(float fDt)
{
	uint32_t lightStride{ 0 };

	auto stage = graphics->getRenderStageID("direct_shadow");
	graphics->bindRenderer(stage);

	// Calculating spot light matrices
	{
		auto lights = registry->view<FTransformComponent, FSpotLightComponent>();
		for (auto [entity, ltransform, light] : lights.each())
		{
			if (!light.castShadows && lightStride < MAX_SPOT_LIGHT_COUNT)
				continue;

			FFrustum frustum;
			const glm::vec3 dp(0.0000001f);

			auto lightDir = light.toTarget ? light.target : glm::normalize(glm::toQuat(ltransform.model) * glm::vec3(0.f, 0.f, 1.f));
			glm::mat4 shadowProj = glm::perspective(light.outerAngle * 2.f, 1.0f, 0.1f, 32.f);
			glm::mat4 shadowView = glm::lookAt(ltransform.rposition, ltransform.position + lightDir, glm::vec3(0.0f, 1.0f, 0.0f));
			auto lightViewProj = shadowProj * shadowView;

			frustum.update(shadowView, shadowProj);

			graphics->bindShader(shader_id);
			graphics->setManualShaderControlFlag(true);

			auto& pUniform = graphics->getUniformHandle("UBOShadowmap");
			pUniform->set("lightViewProj", lightViewProj);
			graphics->flushShader();

			auto meshes = registry->view<FTransformComponent, FMeshComponent>();
			for (auto [entity, mtransform, mesh] : meshes.each())
			{
				auto& head = registry->get<FSceneComponent>(mesh.head);

				if (!graphics->bindVertexBuffer(mesh.vbo_id))
					continue;

				for (auto& meshlet : mesh.vMeshlets)
				{
					auto inLightView = head.castShadows && frustum.checkBox(mtransform.rposition + meshlet.dimensions.min * mtransform.rscale, mtransform.rposition + meshlet.dimensions.max * mtransform.rscale);

					if(inLightView)
					{
						auto& pPush = graphics->getPushBlockHandle("modelData");
						pPush->set("model", mtransform.model);
						pPush->set("stride", lightStride);
						graphics->flushConstantRanges(pPush);

						graphics->draw(meshlet.begin_vertex, meshlet.vertex_count, meshlet.begin_index, meshlet.index_count);
					}
				}

				graphics->bindVertexBuffer(invalid_index);
			}

			graphics->setManualShaderControlFlag(false);
			graphics->bindShader(shader_id);

			lightStride++;
		}
	}

	graphics->bindRenderer(invalid_index);
}