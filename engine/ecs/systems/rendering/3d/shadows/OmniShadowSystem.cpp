#include "OmniShadowSystem.h"

#include "Engine.h"

#include "ecs/components/components.h"

using namespace engine::graphics;
using namespace engine::ecs;

void COmniShadowSystem::__create()
{
	FShaderSpecials specials;
	specials.defines = { {"INVOCATION_COUNT", std::to_string(MAX_POINT_LIGHT_COUNT)} };
	specials.subpass = 1;
	shader_id = EGGraphics->addShader("omni_shadow_pass", "omni_shadow_pass", specials);

	CBaseGraphicsSystem::__create();
}

void COmniShadowSystem::__update(float fDt)
{
	auto& registry = EGCoordinator;

	uint32_t point_light_count{ 0 };
	std::array<glm::mat4, MAX_POINT_LIGHT_COUNT * 6> point_light_matrices;
	std::array<glm::vec4, MAX_POINT_LIGHT_COUNT> point_light_positions;

	// Calculating point light matrices
	{
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 32.f);

		auto view = registry.view<FTransformComponent, FPointLightComponent>();
		for (auto [entity, transform, light] : view.each())
		{
			if (!light.castShadows)
				continue;

			uint32_t array_shift = point_light_count * 6;

			point_light_matrices[point_light_count]		= shadowProj * glm::lookAt(transform.rposition, transform.rposition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)); // POSITIVE_X
			point_light_matrices[point_light_count + 1] = shadowProj * glm::lookAt(transform.rposition, transform.rposition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)); // NEGATIVE_X
			point_light_matrices[point_light_count + 2] = shadowProj * glm::lookAt(transform.rposition, transform.rposition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)); // POSITIVE_Y
			point_light_matrices[point_light_count + 3] = shadowProj * glm::lookAt(transform.rposition, transform.rposition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));	// NEGATIVE_Y
			point_light_matrices[point_light_count + 4] = shadowProj * glm::lookAt(transform.rposition, transform.rposition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));	// POSITIVE_Z
			point_light_matrices[point_light_count + 5] = shadowProj * glm::lookAt(transform.rposition, transform.rposition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));	// NEGATIVE_Z

			point_light_positions[point_light_count++] = glm::vec4(transform.rposition, 1.f);
		}
	}

	if (point_light_count > 0)
	{
		auto commandBuffer = EGGraphics->getCommandBuffer();
		auto& pShader = EGGraphics->getShader(shader_id);

		auto& pUniform = pShader->getUniformBuffer("UBOShadowmap");
		pUniform->set("viewProjMat", point_light_matrices);
		pUniform->set("lightPos", point_light_positions);
		pUniform->set("passedLights", point_light_count);
		pUniform->set("farPlane", 25.f);
		pShader->predraw(commandBuffer);

		auto view = registry.view<FTransformComponent, FMeshComponent>();
		for (auto [entity, transform, mesh] : view.each())
		{
			auto& vbo = EGGraphics->getVertexBuffer(mesh.vbo_id);

			vbo->bind(commandBuffer);

			for (auto& meshlet : mesh.vMeshlets)
			{
				auto& pPush = pShader->getPushBlock("modelData");
				pPush->set("model", transform.model);
				pPush->flush(commandBuffer);

				commandBuffer.drawIndexed(meshlet.index_count, 1, meshlet.begin_index, 0, 0);
			}
		}
	}
}