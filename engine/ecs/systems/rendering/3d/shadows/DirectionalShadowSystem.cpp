#include "DirectionalShadowSystem.h"

#include "Engine.h"

#include "ecs/components/components.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CDirectionalShadowSystem::__create()
{
	FShaderSpecials specials;
	specials.defines = { {"INVOCATION_COUNT", std::to_string(MAX_SPOT_LIGHT_COUNT)} };
	shader_id = EGGraphics->addShader("directional_shadow_pass", "directional_shadow_pass", specials);

	CBaseGraphicsSystem::__create();
}

void CDirectionalShadowSystem::__update(float fDt)
{
	auto& graphics = EGGraphics;
	auto& registry = EGCoordinator;

	uint32_t spot_light_count{ 0 };
	std::array<glm::mat4, MAX_SPOT_LIGHT_COUNT> spot_light_matrices;

	// Calculating spot light matrices
	{
		auto view = registry.view<FTransformComponent, FSpotLightComponent>();
		for (auto [entity, transform, light] : view.each())
		{
			if (!light.castShadows)
				continue;

			const glm::vec3 dp(0.0000001f);

			glm::mat4 shadowProj = glm::perspective(light.innerAngle, 1.0f, 0.1f, 64.f);
			glm::mat4 shadowView = glm::lookAt(transform.rposition + dp, transform.rrotation, glm::vec3(0.0f, 1.0f, 0.0f));

			spot_light_matrices[spot_light_count++] = shadowProj * shadowView;
		}
	}

	if (spot_light_count > 0)
	{
		graphics->bindShader(shader_id);

		auto& pUniform = graphics->getUniformHandle("UBOShadowmap");
		pUniform->set("viewProjMat", spot_light_matrices);
		pUniform->set("passedLights", spot_light_count);
		//pShader->predraw(commandBuffer);

		//auto view = registry.view<FTransformComponent, FMeshComponent>();
		//for (auto [entity, transform, mesh] : view.each())
		//{
		//	graphics->bindVertexBuffer(mesh.vbo_id);
		//
		//	for (auto& meshlet : mesh.vMeshlets)
		//	{
		//		auto& pPush = graphics->getPushBlockHandle("modelData");
		//		pPush->set("model", transform.model);
		//
		//		graphics->draw(meshlet.begin_vertex, meshlet.vertex_count, meshlet.begin_index, meshlet.index_count);
		//	}
		// 
		//  graphics->bindVertexBuffer(invalid_index);
		//}

		graphics->bindShader(shader_id);
	}
}