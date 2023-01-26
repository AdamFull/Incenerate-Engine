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
	shader_id = EGGraphics->addShader("directional_shadow_pass", "directional_shadow_pass", specials);

	CBaseGraphicsSystem::__create();
}

void CDirectionalShadowSystem::__update(float fDt)
{
	auto& graphics = EGGraphics;
	auto& registry = EGCoordinator;

	uint32_t lightStride{ 0 };

	// Calculating spot light matrices
	{
		auto lights = registry.view<FTransformComponent, FSpotLightComponent>();
		for (auto [entity, transform, light] : lights.each())
		{
			if (!light.castShadows && lightStride < MAX_SPOT_LIGHT_COUNT)
				continue;

			FFrustum frustum;
			const glm::vec3 dp(0.0000001f);

			glm::mat4 shadowProj = glm::perspective(light.outerAngle - light.innerAngle, 1.0f, 0.1f, 64.f);
			glm::mat4 shadowView = glm::lookAt(transform.rposition + dp, transform.rrotation, glm::vec3(0.0f, 1.0f, 0.0f));
			auto lightViewProj = shadowProj * shadowView;

			frustum.update(shadowView, shadowProj);

			graphics->bindShader(shader_id);
			graphics->setManualShaderControlFlag(true);

			auto& pUniform = graphics->getUniformHandle("UBOShadowmap");
			pUniform->set("lightViewProj", lightViewProj);
			pUniform->set("stride", lightStride);
			graphics->flushShader();

			auto meshes = registry.view<FTransformComponent, FMeshComponent>();
			for (auto [entity, transform, mesh] : meshes.each())
			{
				graphics->bindVertexBuffer(mesh.vbo_id);

				for (auto& meshlet : mesh.vMeshlets)
				{
					auto inLightView = frustum.checkBox(transform.rposition + meshlet.dimensions.min * transform.rscale, transform.rposition + meshlet.dimensions.max * transform.rscale);

					if(inLightView)
					{
						auto& pPush = graphics->getPushBlockHandle("modelData");
						pPush->set("model", transform.model);
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
}