#include "CompositionSystem.h"

#include "Engine.h"
#include "graphics/image/ImageCubemap.h"
#include "graphics/GraphicsSettings.h"

#include "ecs/components/components.h"

#include "ecs/helper.hpp"

#include <SessionStorage.hpp>

using namespace engine::ecs;
using namespace engine::graphics;

void CCompositionSystem::__create()
{
	auto& device = graphics->getDevice();

	FShaderSpecials specials;
	specials.defines = { 
		{"SHADOW_MAP_CASCADE_COUNT", std::to_string(CASCADE_SHADOW_MAP_CASCADE_COUNT)},
		{"MAX_DIRECTIONAL_LIGHTS_COUNT", std::to_string(MAX_DIRECTIONAL_LIGHT_COUNT)},
		{"MAX_DIRECTIONAL_LIGHT_SHADOW_COUNT", std::to_string(MAX_DIRECTIONAL_LIGHT_SHADOW_COUNT)},

		{"MAX_SPOT_LIGHTS_COUNT", std::to_string(MAX_SPOT_LIGHT_COUNT)},
		{"MAX_SPOT_LIGHT_SHADOW_COUNT", std::to_string(MAX_SPOT_LIGHT_SHADOW_COUNT)},

		{"MAX_POINT_LIGHTS_COUNT", std::to_string(MAX_POINT_LIGHT_COUNT)},
		{"MAX_POINT_LIGHT_SHADOW_COUNT", std::to_string(MAX_POINT_LIGHT_SHADOW_COUNT)},
	};
	shader_id = graphics->addShader("pbr_composition", specials);
	brdflut_id = graphics->computeBRDFLUT(1024);

	addSubresource("albedo_tex");
	addSubresource("normal_tex");
	addSubresource("mrah_tex");
	addSubresource("emission_tex");
	addSubresource("depth_tex");

	addSubresource("raw_ao_tex");
	//addSubresource("global_illumination_tex");
	//addSubresource("reflections_tex");
	
	addSubresource("cascade_shadowmap_tex");
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
	auto& settings = CGraphicsSettings::getInstance()->getSettings();

	auto& shadowManager = EGEngine->getShadows();
	auto& lightsManager = EGEngine->getLights();

	auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	auto& directional = lightsManager->getDirectional();
	auto& point = lightsManager->getPoint();
	auto& spot = lightsManager->getSpot();

	auto stage = graphics->getRenderStageID("composition");

	auto eskybox = get_active_skybox(*registry);
	auto* skybox = registry->try_get<FEnvironmentComponent>(eskybox);

	graphics->bindShader(shader_id);

	// Binding ibl images
	graphics->bindTexture("brdflut_tex", brdflut_id);
	graphics->bindTexture("irradiance_tex", skybox ? skybox->irradiance : empty_cube_id);
	graphics->bindTexture("prefiltred_tex", skybox ? skybox->prefiltred : empty_cube_id);

	//graphics->bindTexture("albedo_tex", settings.bEnableReflections ? getSubresource("reflections_tex") : getSubresource("albedo_tex"));
	graphics->bindTexture("albedo_tex", getSubresource("albedo_tex"));
	graphics->bindTexture("normal_tex", getSubresource("normal_tex"));
	graphics->bindTexture("mrah_tex", getSubresource("mrah_tex"));
	graphics->bindTexture("emission_tex", getSubresource("emission_tex"));
	graphics->bindTexture("depth_tex", getSubresource("depth_tex"));

	graphics->bindTexture("ambient_occlusion_tex", graphics->getImageID("ambient_occlusion_tex"));
	//graphics->bindTexture("global_illumination_tex", getSubresource("global_illumination_tex"));

	graphics->bindTexture("cascade_shadowmap_tex", getSubresource("cascade_shadowmap_tex"));
	graphics->bindTexture("direct_shadowmap_tex", getSubresource("direct_shadowmap_tex"));
	graphics->bindTexture("omni_shadowmap_tex", getSubresource("omni_shadowmap_tex"));

	auto& pUBO = graphics->getUniformHandle("UBODeferred");
	pUBO->set("view", camera->view);
	pUBO->set("invViewProjection", glm::inverse(camera->projection * camera->view));
	pUBO->set("viewPos", glm::vec4(camera->viewPos, 1.0));
	pUBO->set("directionalLightCount", directional.light_count);
	pUBO->set("spotLightCount", spot.light_count);
	pUBO->set("pointLightCount", point.light_count);
	pUBO->set("ambientOcclusion", settings.bEnableAmbientOcclusion ? 1 : -1);

	auto& pUBOLights = graphics->getUniformHandle("UBOLights");
	pUBOLights->set("directionalLights", directional.lights);
	pUBOLights->set("spotLights", spot.lights);
	pUBOLights->set("pointLights", point.lights);

	// Push shadow data
	auto& pUBOShadows = graphics->getUniformHandle("UBOShadows");
	pUBOShadows->set("directionalShadows", shadowManager->getDirectionalLightShadows());
	pUBOShadows->set("spotShadows", shadowManager->getSpotLightShadows());
	pUBOShadows->set("pointShadows", shadowManager->getPointLightShadows());

	// TODO: get this data from editor
	auto& pUBODebug = graphics->getUniformHandle("UBODebug");
	pUBODebug->set("mode", 0);
	pUBODebug->set("cascadeSplit", 0);
	pUBODebug->set("spotShadowIndex", 0);
	pUBODebug->set("omniShadowIndex", 0);
	pUBODebug->set("omniShadowView", 0);
	pUBODebug->set("mode", CSessionStorage::getInstance()->get<int32_t>("render_debug_mode"));
	pUBODebug->set("cascadeSplit", CSessionStorage::getInstance()->get<int32_t>("render_debug_cascade_split"));
	pUBODebug->set("spotShadowIndex", CSessionStorage::getInstance()->get<int32_t>("render_debug_spot_shadow_index"));
	pUBODebug->set("omniShadowIndex", CSessionStorage::getInstance()->get<int32_t>("render_debug_omni_shadow_index"));
	pUBODebug->set("omniShadowView", CSessionStorage::getInstance()->get<int32_t>("render_debug_omni_shadow_view"));

	// Drawing 
	graphics->bindRenderer(stage);
	graphics->flushShader();
	graphics->draw(0, 3, 0, 0, 1);
	graphics->bindRenderer(invalid_index);
}