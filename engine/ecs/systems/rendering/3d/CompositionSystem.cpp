#include "CompositionSystem.h"

#include "Engine.h"
#include "graphics/image/ImageCubemap.h"

#include "ecs/components/components.h"

#include "ecs/helper.hpp"

using namespace engine::ecs;
using namespace engine::graphics;

void CCompositionSystem::__create()
{
	auto& device = graphics->getDevice();
	shader_id = graphics->addShader("pbr_composition", "pbr_composition");
	brdflut_id = graphics->computeBRDFLUT(512);

	addSubresource("albedo_tex");
	addSubresource("normal_tex");
	addSubresource("mrah_tex");
	addSubresource("emission_tex");
	addSubresource("depth_tex");
	//addSubresource("picking_tex");
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

	empty_cube_id = graphics->addImage("empty_cubemap", std::move(emptyCubemap));

	CBaseGraphicsSystem::__create();
}

void CCompositionSystem::__update(float fDt)
{
	uint32_t directoonal_light_count{ 0 };
	std::array<FDirectionalLightCommit, MAX_DIRECTIONAL_LIGHT_COUNT> directional_lights;
	uint32_t point_light_count{ 0 };
	std::array<FPointLightCommit, MAX_POINT_LIGHT_COUNT> point_lights;
	uint32_t spot_light_count{ 0 };
	std::array<FSpotLightCommit, MAX_SPOT_LIGHT_COUNT> spot_lights;

	// Collecting directional lights
	{
		auto view = registry->view<FTransformComponent, FDirectionalLightComponent>();
		for (auto [entity, transform, light] : view.each())
		{
			FDirectionalLightCommit commit;
			commit.direction = glm::normalize(glm::toQuat(transform.model) * glm::vec3(0.f, 0.f, 1.f));
			commit.color = light.color;
			commit.intencity = light.intencity;
			commit.castShadows = static_cast<int>(light.castShadows);

			directional_lights[directoonal_light_count++] = commit;
		}
	}
	
	// Collecting point lights
	{
		auto view = registry->view<FTransformComponent, FPointLightComponent>();
		for (auto [entity, transform, light] : view.each())
		{
			FPointLightCommit commit;
			commit.position = transform.rposition;
			commit.color = light.color;
			commit.intencity = light.intencity;
			commit.radius = light.radius;
			commit.castShadows = static_cast<int>(light.castShadows);

			point_lights[point_light_count++] = commit;
		}
	}

	// Collecting spot lights
	{
		// TODO: direction as dot product of rotation and some vec
		auto view = registry->view<FTransformComponent, FSpotLightComponent>();
		for (auto [entity, transform, light] : view.each())
		{
			FSpotLightCommit commit;
			commit.position = transform.rposition; 
			commit.direction = light.toTarget ? light.target : glm::normalize(glm::toQuat(transform.model) * glm::vec3(0.f, 0.f, 1.f));
			commit.color = light.color;
			commit.intencity = light.intencity;
			commit.lightAngleScale = 1.f / glm::max(0.001f, glm::cos(light.innerAngle) - glm::cos(light.outerAngle));
			commit.lightAngleOffset = -glm::cos(light.outerAngle) * commit.lightAngleScale;
			commit.toTarget = static_cast<int>(light.toTarget);
			commit.castShadows = static_cast<int>(light.castShadows);

			spot_lights[spot_light_count++] = commit;
		}
	}

	auto stage = graphics->getRenderStageID("composition");

	auto eskybox = get_active_skybox(*registry);
	auto* skybox = registry->try_get<FEnvironmentComponent>(eskybox);

	auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	graphics->bindShader(shader_id);

	// Binding ibl images
	graphics->bindTexture("brdflut_tex", brdflut_id);
	graphics->bindTexture("irradiance_tex", skybox ? skybox->irradiance : empty_cube_id);
	graphics->bindTexture("prefiltred_tex", skybox ? skybox->prefiltred : empty_cube_id);

	graphics->bindTexture("albedo_tex", getSubresource("albedo_tex"));
	graphics->bindTexture("normal_tex", getSubresource("normal_tex"));
	graphics->bindTexture("mrah_tex", getSubresource("mrah_tex"));
	graphics->bindTexture("emission_tex", getSubresource("emission_tex"));
	graphics->bindTexture("depth_tex", getSubresource("depth_tex"));
	//graphics->bindTexture("picking_tex", getSubresource("picking_tex"));

	graphics->bindTexture("direct_shadowmap_tex", getSubresource("direct_shadowmap_tex"));
	graphics->bindTexture("omni_shadowmap_tex", getSubresource("omni_shadowmap_tex"));

	auto& pUBO = graphics->getUniformHandle("UBODeferred");
	pUBO->set("invViewProjection", glm::inverse(camera->projection * camera->view));
	pUBO->set("viewPos", glm::vec4(camera->viewPos, 1.0));
	pUBO->set("directionalLightCount", directoonal_light_count);
	pUBO->set("spotLightCount", spot_light_count);
	pUBO->set("pointLightCount", point_light_count);

	auto& pUBOLights = graphics->getUniformHandle("UBOLights");
	pUBOLights->set("directionalLights", directional_lights);
	pUBOLights->set("spotLights", spot_lights);
	pUBOLights->set("pointLights", point_lights);

	// Drawing 
	graphics->bindRenderer(stage);
	graphics->draw(0, 3, 0, 0, 1);
	graphics->bindRenderer(invalid_index);
}