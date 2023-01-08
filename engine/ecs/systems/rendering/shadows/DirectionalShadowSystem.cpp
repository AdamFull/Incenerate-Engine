#include "DirectionalShadowSystem.h"

#include "Engine.h"

#include "ecs/components/SpotLightComponent.h"
#include "ecs/components/MeshComponent.h"

using namespace engine::ecs;

void CDirectionalShadowSystem::__create()
{
	shader_id = EGGraphics->addShader("directional_shadow_pass", "directional_shadow_pass", { {"INVOCATION_COUNT", std::to_string(MAX_SPOT_LIGHT_COUNT)} });
}

void CDirectionalShadowSystem::__update(float fDt)
{
	uint32_t spot_light_count{ 0 };
	std::array<glm::mat4, MAX_SPOT_LIGHT_COUNT> spot_light_matrices;

	// Calculating spot light matrices
	{
		auto view = EGCoordinator.view<FSpotLightComponent>();
		for (auto [entity, light] : view.each())
		{
			const glm::vec3 dp(0.0000001f);
			auto& pNode = EGSceneGraph->find(entity, true);
			auto transform = pNode->getTransform();

			glm::mat4 shadowProj = glm::perspective(light.innerAngle, 1.0f, 0.1f, 64.f);
			glm::mat4 shadowView = glm::lookAt(transform.position + dp, transform.rotation, glm::vec3(0.0f, 1.0f, 0.0f));

			spot_light_matrices[spot_light_count++] = shadowProj * shadowView;
		}
	}

	auto commandBuffer = EGGraphics->getCommandBuffer();
	auto& pShader = EGGraphics->getShader(shader_id);

	auto& pUniform = pShader->getUniformBuffer("UBOShadowmap");
	pUniform->set("viewProjMat", spot_light_matrices);
	pUniform->set("passedLights", spot_light_count);
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

	commandBuffer.nextSubpass(vk::SubpassContents::eInline);
}