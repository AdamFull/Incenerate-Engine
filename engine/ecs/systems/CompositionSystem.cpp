#include "CompositionSystem.h"

#include "Engine.h"

#include "ecs/components/SkyboxComponent.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/DirectionalLightComponent.h"
#include "ecs/components/SpotLightComponent.h"
#include "ecs/components/PointLightComponent.h"

using namespace engine::ecs;
using namespace engine::graphics;

void CCompositionSystem::__create()
{
	shader_id = EGGraphics->addShader("pbr_composition", "pbr_composition");
	
}

void CCompositionSystem::__update(float fDt)
{
	uint32_t directoonal_light_count{ 0 };
	std::array<FDirectionalLightCommit, 1> directional_lights;
	uint32_t point_light_count{ 0 };
	std::array<FPointLightCommit, 16> point_lights;
	uint32_t spot_light_count{ 0 };
	std::array<FSpotLightCommit, 15> spot_lights;

	auto& stage = EGGraphics->getRenderStage("composition");
	auto commandBuffer = EGGraphics->getCommandBuffer();
	auto& pShader = EGGraphics->getShader(shader_id);
	auto index = EGGraphics->getDevice()->getCurrentFrame();

	auto activeSkybox = EGCoordinator->getActiveEnvironment();
	if (activeSkybox)
	{
		auto& skybox = EGCoordinator->getComponent<FSkyboxComponent>(activeSkybox.value());

		stage->begin(commandBuffer);

		pShader->addTexture("brdflut_tex", skybox.brdflut);
		pShader->addTexture("irradiance_tex", skybox.irradiance);
		pShader->addTexture("prefiltred_tex", skybox.prefiltred);

		auto& packed = EGGraphics->getImage("packed_tex_" + std::to_string(index));
		pShader->addTexture("packed_tex", packed->getDescriptor());

		auto& emission = EGGraphics->getImage("emission_tex_" + std::to_string(index));
		pShader->addTexture("emission_tex", emission->getDescriptor());

		auto& depth = EGGraphics->getImage("depth_tex_" + std::to_string(index));
		pShader->addTexture("depth_tex", depth->getDescriptor());

		auto activeCamera = EGCoordinator->getActiveCamera();
		if (activeCamera)
		{
			auto& camera = EGCoordinator->getComponent<FCameraComponent>(activeCamera.value());
			auto& cameraTransform = EGCoordinator->getComponent<FTransformComponent>(activeCamera.value());
			auto invViewProjection = glm::inverse(camera.projection * camera.view);

			auto& pUBO = pShader->getUniformBuffer("UBODeferred");
			pUBO->set("invViewProjection", invViewProjection);
			pUBO->set("view", invViewProjection);
			pUBO->set("viewPos", cameraTransform.position);
			pUBO->set("directionalLightCount", directoonal_light_count);
			pUBO->set("spotLightCount", spot_light_count);
			pUBO->set("pointLightCount", point_light_count);

			auto& pUBOLights = pShader->getUniformBuffer("UBOLights");
			pUBOLights->set("directionalLights", directional_lights);
			pUBOLights->set("spotLights", spot_lights);
			pUBOLights->set("pointLights", point_lights);

			pShader->predraw(commandBuffer);

			commandBuffer.draw(3, 1, 0, 0);
		}

		stage->end(commandBuffer);
	}
}