#include "OmniShadowSystem.h"

#include "Engine.h"

#include "ecs/components/PointLightComponent.h"
#include "ecs/components/MeshComponent.h"

using namespace engine::ecs;

void COmniShadowSystem::__create()
{
	shader_id = EGGraphics->addShader("omni_shadow_pass", "omni_shadow_pass", { {"INVOCATION_COUNT", std::to_string(MAX_POINT_LIGHT_COUNT)} }, 1);
}

void COmniShadowSystem::__update(float fDt)
{
	uint32_t point_light_count{ 0 };
	std::array<glm::mat4, MAX_POINT_LIGHT_COUNT * 6> point_light_matrices;
	std::array<glm::vec4, MAX_POINT_LIGHT_COUNT> point_light_positions;

	// Calculating point light matrices
	{
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 32.f);

		auto view = EGCoordinator.view<FPointLightComponent>();
		for (auto [entity, light] : view.each())
		{
			auto& pNode = EGSceneGraph->find(entity, true);
			auto transform = pNode->getTransform();

			uint32_t array_shift = point_light_count * 6;

			point_light_matrices[point_light_count]		= shadowProj * glm::lookAt(transform.position, transform.position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)); // POSITIVE_X
			point_light_matrices[point_light_count + 1] = shadowProj * glm::lookAt(transform.position, transform.position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)); // NEGATIVE_X
			point_light_matrices[point_light_count + 2] = shadowProj * glm::lookAt(transform.position, transform.position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)); // POSITIVE_Y
			point_light_matrices[point_light_count + 3] = shadowProj * glm::lookAt(transform.position, transform.position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));	// NEGATIVE_Y
			point_light_matrices[point_light_count + 4] = shadowProj * glm::lookAt(transform.position, transform.position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));	// POSITIVE_Z
			point_light_matrices[point_light_count + 5] = shadowProj * glm::lookAt(transform.position, transform.position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));	// NEGATIVE_Z

			point_light_positions[point_light_count++] = glm::vec4(transform.position, 1.f);
		}
	}

	auto commandBuffer = EGGraphics->getCommandBuffer();
	auto& pShader = EGGraphics->getShader(shader_id);

	auto& pUniform = pShader->getUniformBuffer("UBOShadowmap");
	pUniform->set("viewProjMat", point_light_matrices);
	pUniform->set("lightPos", point_light_positions);
	pUniform->set("passedLights", point_light_count);
	pUniform->set("farPlane", 25.f);
	pShader->predraw(commandBuffer);

	auto view = EGCoordinator.view<FMeshComponent>();
	for (auto [entity, mesh] : view.each())
	{
		auto& vbo = EGGraphics->getVertexBuffer(mesh.vbo_id);

		auto& pNode = EGSceneGraph->find(entity, true);
		auto model = pNode->getModel();

		vbo->bind(commandBuffer);

		for (auto& meshlet : mesh.vMeshlets)
		{
			auto& pPush = pShader->getPushBlock("modelData");
			pPush->set("model", model);
			pPush->flush(commandBuffer);

			commandBuffer.drawIndexed(meshlet.index_count, 1, meshlet.begin_index, 0, 0);
		}
	}
}