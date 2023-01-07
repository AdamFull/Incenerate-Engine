#include "CompositionSystem.h"

#include "Engine.h"

#include "ecs/components/SkyboxComponent.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/DirectionalLightComponent.h"
#include "ecs/components/SpotLightComponent.h"
#include "ecs/components/PointLightComponent.h"

#include "ecs/helper.hpp"

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

	// Collecting directional lights
	{
		auto view = EGCoordinator.view<FTransformComponent, FDirectionalLightComponent>();
		for (auto [entity, transform, light] : view.each())
		{
			FDirectionalLightCommit commit;
			commit.direction = transform.rotation;
			commit.color = light.color;
			commit.intencity = light.intencity;

			directional_lights[directoonal_light_count++] = commit;
		}
	}
	
	// Collecting point lights
	{
		auto view = EGCoordinator.view<FTransformComponent, FPointLightComponent>();
		for (auto [entity, transform, light] : view.each())
		{
			FPointLightCommit commit;
			commit.position = transform.position;
			commit.color = light.color;
			commit.intencity = light.intencity;
			commit.radius = light.radius;

			point_lights[point_light_count++] = commit;
		}
	}

	// Collecting spot lights
	{
		auto view = EGCoordinator.view<FTransformComponent, FSpotLightComponent>();
		for (auto [entity, transform, light] : view.each())
		{
			FSpotLightCommit commit;
			commit.position = transform.position;
			commit.direction = transform.rotation;
			commit.color = light.color;
			commit.intencity = light.intencity;
			commit.innerAngle = light.innerAngle;
			commit.outerAngle = light.outerAngle;

			spot_lights[spot_light_count++] = commit;
		}
	}

	auto& stage = EGGraphics->getRenderStage("composition");
	auto commandBuffer = EGGraphics->getCommandBuffer();
	auto& pShader = EGGraphics->getShader(shader_id);
	auto index = EGGraphics->getDevice()->getCurrentFrame();

	// TODO: add empty or default skybox
	auto eskybox = get_active_skybox(EGCoordinator);
	auto& skybox = EGCoordinator.get<FSkyboxComponent>(eskybox);

	auto ecamera = get_active_camera(EGCoordinator);
	auto& camera = EGCoordinator.get<FCameraComponent>(ecamera);
	auto& cameraTransform = EGCoordinator.get<FTransformComponent>(ecamera);
	auto invViewProjection = glm::inverse(camera.projection * camera.view);

	// Setting up predraw data
	pShader->addTexture("brdflut_tex", skybox.brdflut);
	pShader->addTexture("irradiance_tex", skybox.irradiance);
	pShader->addTexture("prefiltred_tex", skybox.prefiltred);

	auto& packed = EGGraphics->getImage("packed_tex_" + std::to_string(index));
	pShader->addTexture("packed_tex", packed->getDescriptor());

	auto& emission = EGGraphics->getImage("emission_tex_" + std::to_string(index));
	pShader->addTexture("emission_tex", emission->getDescriptor());

	auto& depth = EGGraphics->getImage("depth_tex_" + std::to_string(index));
	pShader->addTexture("depth_tex", depth->getDescriptor());

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

	// Drawing 
	stage->begin(commandBuffer);
	pShader->predraw(commandBuffer);
	commandBuffer.draw(3, 1, 0, 0);
	stage->end(commandBuffer);
}