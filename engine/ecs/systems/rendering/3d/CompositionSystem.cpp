#include "CompositionSystem.h"

#include "Engine.h"
#include "graphics/image/ImageCubemap.h"

#include "ecs/components/EnvironmentComponent.h"
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
	auto& device = EGGraphics->getDevice();
	shader_id = EGGraphics->addShader("pbr_composition", "pbr_composition");
	brdflut_id = EGGraphics->computeBRDFLUT(512);

	auto emptyCubemap = std::make_unique<CImageCubemap>(device.get());
	emptyCubemap->create(
		vk::Extent2D{ 16, 16 },
		vk::Format::eR32G32B32A32Sfloat,
		vk::ImageLayout::eGeneral,
		vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
		vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment |
		vk::ImageUsageFlagBits::eStorage);

	empty_cube_id = EGGraphics->addImage("empty_cubemap", std::move(emptyCubemap));
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
			commit.direction = transform.rrotation;
			commit.color = light.color;
			commit.intencity = light.intencity;

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

			point_lights[point_light_count++] = commit;
		}
	}

	// Collecting spot lights
	{
		auto view = registry.view<FTransformComponent, FSpotLightComponent>();
		for (auto [entity, transform, light] : view.each())
		{
			FSpotLightCommit commit;
			commit.position = transform.rposition;
			commit.direction = transform.rrotation;
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

		auto& albedo = EGGraphics->getImage("albedo_tex_" + std::to_string(index));
		pShader->addTexture("albedo_tex", albedo->getDescriptor());

		auto& normal = EGGraphics->getImage("normal_tex_" + std::to_string(index));
		pShader->addTexture("normal_tex", normal->getDescriptor());

		auto& mrah = EGGraphics->getImage("mrah_tex_" + std::to_string(index));
		pShader->addTexture("mrah_tex", mrah->getDescriptor());

		auto& emission = EGGraphics->getImage("emission_tex_" + std::to_string(index));
		pShader->addTexture("emission_tex", emission->getDescriptor());

		auto& depth = EGGraphics->getImage("depth_tex_" + std::to_string(index));
		pShader->addTexture("depth_tex", depth->getDescriptor());

		auto& directsm = EGGraphics->getImage("direct_shadowmap_tex_" + std::to_string(index));
		pShader->addTexture("direct_shadowmap_tex", directsm->getDescriptor());

		auto& omnism = EGGraphics->getImage("omni_shadowmap_tex_" + std::to_string(index));
		pShader->addTexture("omni_shadowmap_tex", omnism->getDescriptor());

		auto& pUBO = pShader->getUniformBuffer("UBODeferred");
		pUBO->set("invViewProjection", glm::inverse(camera->projection * camera->view));
		pUBO->set("invProjMatrix", glm::inverse(camera->projection)); // TODO: delete
		pUBO->set("invViewMatrix", glm::inverse(camera->view)); // TODO: delete
		pUBO->set("view", camera->view); // TODO: delete
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