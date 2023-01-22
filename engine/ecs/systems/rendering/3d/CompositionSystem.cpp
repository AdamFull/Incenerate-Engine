#include "CompositionSystem.h"

#include "Engine.h"
#include "graphics/image/ImageCubemap.h"

#include "ecs/components/components.h"

#include "ecs/helper.hpp"

using namespace engine::ecs;
using namespace engine::graphics;

void CCompositionSystem::__create()
{
	auto& device = EGGraphics->getDevice();
	shader_id = EGGraphics->addShader("pbr_composition", "pbr_composition");
	brdflut_id = EGGraphics->computeBRDFLUT(512);

	addSubresource("albedo_tex");
	addSubresource("normal_tex");
	addSubresource("mrah_tex");
	addSubresource("emission_tex");
	addSubresource("depth_tex");
	addSubresource("direct_shadowmap_tex");
	addSubresource("omni_shadowmap_tex");

	auto emptyCubemap = std::make_unique<CImageCubemap>(device.get());
	emptyCubemap->create(
		vk::Extent2D{ 16, 16 },
		vk::Format::eR32G32B32A32Sfloat,
		vk::ImageLayout::eGeneral,
		vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
		vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment |
		vk::ImageUsageFlagBits::eStorage);

	empty_cube_id = EGGraphics->addImage("empty_cubemap", std::move(emptyCubemap));

	CBaseGraphicsSystem::__create();
}

void CCompositionSystem::__update(float fDt)
{
	auto& registry = EGCoordinator;
	auto editorMode = EGEngine->isEditorMode();
	auto state = EGEngine->getState();

	uint32_t directoonal_light_count{ 0 };
	std::array<FDirectionalLightCommit, MAX_DIRECTIONAL_LIGHT_COUNT> directional_lights;
	uint32_t point_light_count{ 0 };
	std::array<FPointLightCommit, MAX_POINT_LIGHT_COUNT> point_lights;
	uint32_t spot_light_count{ 0 };
	std::array<FSpotLightCommit, MAX_SPOT_LIGHT_COUNT> spot_lights;

	// Collecting directional lights
	{
		auto view = registry.view<FTransformComponent, FDirectionalLightComponent>();
		for (auto [entity, transform, light] : view.each())
		{
			FDirectionalLightCommit commit;
			commit.direction = glm::normalize(transform.rrotation * light.direction);
			commit.color = light.color;
			commit.intencity = light.intencity;
			commit.castShadows = light.castShadows;

			directional_lights[directoonal_light_count++] = commit;
		}
	}
	
	// Collecting point lights
	{
		auto view = registry.view<FTransformComponent, FPointLightComponent>();
		for (auto [entity, transform, light] : view.each())
		{
			FPointLightCommit commit;
			commit.position = transform.rposition;
			commit.color = light.color;
			commit.intencity = light.intencity;
			commit.radius = light.radius;
			commit.castShadows = light.castShadows;

			point_lights[point_light_count++] = commit;
		}
	}

	// Collecting spot lights
	{
		// TODO: direction as dot product of rotation and some vec
		auto view = registry.view<FTransformComponent, FSpotLightComponent>();
		for (auto [entity, transform, light] : view.each())
		{
			FSpotLightCommit commit;
			commit.position = transform.rposition;
			commit.direction = transform.rrotation * light.direction;
			commit.color = light.color;
			commit.intencity = light.intencity;
			commit.lightAngleScale = 1.f / glm::max(0.001f, glm::cos(light.innerAngle) - glm::cos(light.outerAngle));
			commit.lightAngleOffset = -glm::cos(light.outerAngle) * commit.lightAngleScale;
			commit.toTarget = light.toTarget;
			commit.castShadows = light.castShadows;

			spot_lights[spot_light_count++] = commit;
		}
	}

	auto& stage = EGGraphics->getRenderStage("composition");
	auto commandBuffer = EGGraphics->getCommandBuffer();
	auto& pShader = EGGraphics->getShader(shader_id);
	auto index = EGGraphics->getDevice()->getCurrentFrame();

	auto eskybox = get_active_skybox(registry);
	FEnvironmentComponent* skybox = registry.try_get<FEnvironmentComponent>(eskybox);

	FCameraComponent* camera{ nullptr };

	if (editorMode && state == EEngineState::eEditing)
		camera = registry.try_get<FCameraComponent>(EGEditor->getCamera());
	else
		camera = registry.try_get<FCameraComponent>(get_active_camera(registry));

	if (camera)
	{
		// Setting up predraw data
		pShader->addTexture("brdflut_tex", brdflut_id);
		pShader->addTexture("irradiance_tex", skybox ? skybox->irradiance : empty_cube_id);
		pShader->addTexture("prefiltred_tex", skybox ? skybox->prefiltred : empty_cube_id);

		pShader->addTexture("albedo_tex", getSubresource("albedo_tex"));
		pShader->addTexture("normal_tex", getSubresource("normal_tex"));
		pShader->addTexture("mrah_tex", getSubresource("mrah_tex"));
		pShader->addTexture("emission_tex", getSubresource("emission_tex"));
		pShader->addTexture("depth_tex", getSubresource("depth_tex"));

		pShader->addTexture("direct_shadowmap_tex", getSubresource("direct_shadowmap_tex"));
		pShader->addTexture("omni_shadowmap_tex", getSubresource("omni_shadowmap_tex"));

		auto& pUBO = pShader->getUniformBuffer("UBODeferred");
		pUBO->set("invViewProjection", glm::inverse(camera->projection * camera->view));
		pUBO->set("viewPos", glm::vec4(camera->viewPos, 1.0));
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
}