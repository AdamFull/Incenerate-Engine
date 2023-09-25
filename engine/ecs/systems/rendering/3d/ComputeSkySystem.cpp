#include "ComputeSkySystem.h"

#include "Engine.h"
#include "graphics/image/Image2D.h"
#include "graphics/image/Image3D.h"
#include "postprocess/EffectShared.h"

#include "ecs/components/components.h"

#include "ecs/helper.hpp"
#include <Helpers.h>

using namespace engine::ecs;
using namespace engine::graphics;

void CComputeSkySystem::__create()
{
	FShaderCreateInfo specials;
	specials.bind_point = vk::PipelineBindPoint::eCompute;
	specials.usages = 1;

	// TODO: Set this value as constants
	//specials.defines = {
	//	{"MAX_DIRECTIONAL_LIGHTS_COUNT", std::to_string(MAX_DIRECTIONAL_LIGHT_COUNT)}
	//};
	atmosphere_shader_id = graphics->addShader("skybox:atmosphere", specials);
	atmosphere_tex_id = effectshared::createImage("sky_atmosphere_texture", vk::Format::eB10G11R11UfloatPack32);

	//clouds_shader_id = graphics->addShader("sky_clouds_comp");
	//clouds_tex_id = effectshared::createImage("sky_clouds_texture", vk::Format::eR16G16B16A16Sfloat);
	//
	//sun_shader_id = graphics->addShader("sky_sun_comp");
	//sun_tex_id = effectshared::createImage("sky_sun_texture", vk::Format::eR16G16B16A16Sfloat);
	//
	//composition_shader_id = graphics->addShader("sky_compose_comp");
	//composition_tex_id = effectshared::createImage("sky_compose_texture", vk::Format::eR16G16B16A16Sfloat);
	//
	//perlin_noise_tex_id = graphics->compute3DNoise("cloudsNoiseHigh", "genCloudsNoiseHigh", vk::Extent3D{ 32, 32, 32 });
	//cellular_noise_tex_id = graphics->compute3DNoise("cloudsNoiseLow", "genCloudsNoiseLow", vk::Extent3D{ 128, 128, 128 });

	//noise_tex_id = graphics->compute2DNoise("shared_noise_tex", vk::Extent2D{ 256, 256 });

	CBaseGraphicsSystem::__create();
}

void CComputeSkySystem::__update(float fDt)
{
	fTime += fDt;

	auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	{
		effectshared::tryReCreateImage("sky_atmosphere_texture", atmosphere_tex_id, vk::Format::eB10G11R11UfloatPack32);
		//effectshared::tryReCreateImage("sky_clouds_texture", clouds_tex_id, vk::Format::eR16G16B16A16Sfloat);
		//effectshared::tryReCreateImage("sky_sun_texture", sun_tex_id, vk::Format::eR16G16B16A16Sfloat);
		//effectshared::tryReCreateImage("sky_compose_texture", composition_tex_id, vk::Format::eR16G16B16A16Sfloat);
	}

	if (cloudmap_tex_id == invalid_index)
	{
		cloudmap_tex_id = graphics->addImage("cloudmap", "/assets/textures/skybox/cloudmap.png");
	}

	if (noise_tex_id == invalid_index)
	{
		noise_tex_id = graphics->addImage("shared_noise_tex", "/assets/textures/skybox/noise.png");
	}

	computeAtmosphericScattering(camera->projection, camera->view);
	//graphics->BarrierFromComputeToCompute();
	//computeClouds(camera->projection, camera->view);
	//graphics->BarrierFromComputeToCompute();
	//computeComposition(camera->projection, camera->view);
	graphics->BarrierFromComputeToGraphics();
}

void CComputeSkySystem::computeAtmosphericScattering(const glm::mat4& projection, const glm::mat4& view)
{
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);

	FDispatchParam param{};
	param.size = {extent.width, extent.height, 1};

	auto& lightsManager = EGEngine->getLights();
	auto& directional = lightsManager->getDirectional();

	auto invView = glm::inverse(view);
	auto invProjection = glm::inverse(projection);
	auto invViewProjection = glm::inverse(projection * view);

	graphics->bindShader(atmosphere_shader_id);

	graphics->bindTexture("writeColor", atmosphere_tex_id);

	auto& pCamera = graphics->getUniformHandle("FCameraData");
	pCamera->set("view", view);
	pCamera->set("invView", invView);
	pCamera->set("projection", projection);
	pCamera->set("invProjection", invProjection);
	pCamera->set("invViewProjection", invViewProjection);

	auto& pScattering = graphics->getUniformHandle("FAtmosphereScattering");
	pScattering->set("lightCount", directional.light_count);

	auto& pLights = graphics->getUniformHandle("UBOLights");
	pLights->set("directionalLights", directional.lights);

	graphics->dispatch(param);

	graphics->bindShader(invalid_index);
}

void CComputeSkySystem::computeClouds(const glm::mat4 & projection, const glm::mat4 & view)
{
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);

	FDispatchParam param{};
	param.size = { extent.width, extent.height, 1 };

	auto& lightsManager = EGEngine->getLights();
	auto& directional = lightsManager->getDirectional();

	auto invView = glm::inverse(view);
	auto invProjection = glm::inverse(projection);
	auto invViewProjection = glm::inverse(projection * view);

	graphics->bindShader(clouds_shader_id);

	auto& pCamera = graphics->getUniformHandle("FCameraData");
	pCamera->set("view", view);
	pCamera->set("invView", invView);
	pCamera->set("projection", projection);
	pCamera->set("invProjection", invProjection);
	pCamera->set("invViewProjection", invViewProjection);
	pCamera->set("time", fTime);

	auto& pClouds = graphics->getUniformHandle("FCloudsMarching");
	pClouds->set("cloudsDensity", 0.48f);
	pClouds->set("cloudsCoverage", 0.4f);
	pClouds->set("scatteringSteps", 5);
	pClouds->set("scatteringDensity", 0.5f);
	pClouds->set("scatteringIntensity", 0.5f);
	pClouds->set("scatteringPhase", 0.5f);
	pClouds->set("phaseInfluence1", 0.025f);
	pClouds->set("phaseInfluence2", 0.9f);
	pClouds->set("eccentrisy1", 0.95f);
	pClouds->set("eccentrisy2", 0.51f);
	pClouds->set("cloudsAttenuation1", 0.3f);
	pClouds->set("cloudsAttenuation2", 0.06f);
	pClouds->set("fog", 10.f);
	pClouds->set("ambient", 0.5f);

	auto& pScattering = graphics->getUniformHandle("FAtmosphereScattering");
	pScattering->set("lightCount", directional.light_count);

	auto& pLights = graphics->getUniformHandle("UBOLights");
	pLights->set("directionalLights", directional.lights);

	graphics->bindTexture("writeColor", clouds_tex_id);
	graphics->bindTexture("atmosphereTex", atmosphere_tex_id);
	graphics->bindTexture("cloudsMap", cloudmap_tex_id);
	graphics->bindTexture("cloudsNoiseHighSampler", perlin_noise_tex_id);
	graphics->bindTexture("cloudsNoiseLowSampler", cellular_noise_tex_id);
	graphics->bindTexture("noiseSampler", noise_tex_id);

	graphics->dispatch(param);

	graphics->bindShader(invalid_index);
}

void CComputeSkySystem::computeSun(const glm::mat4& projection, const glm::mat4& view)
{
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);

	FDispatchParam param{};
	param.size = { extent.width, extent.height, 1 };

	auto& lightsManager = EGEngine->getLights();
	auto& directional = lightsManager->getDirectional();

	auto invView = glm::inverse(view);
	auto invProjection = glm::inverse(projection);
	auto invViewProjection = glm::inverse(projection * view);

	graphics->bindShader(sun_shader_id);

	auto& pCamera = graphics->getUniformHandle("FCameraData");
	pCamera->set("view", view);
	pCamera->set("invView", invView);
	pCamera->set("projection", projection);
	pCamera->set("invProjection", invProjection);
	pCamera->set("invViewProjection", invViewProjection);

	auto& pScattering = graphics->getUniformHandle("FAtmosphereScattering");
	pScattering->set("lightCount", directional.light_count);

	auto& pLights = graphics->getUniformHandle("UBOLights");
	pLights->set("directionalLights", directional.lights);

	graphics->bindTexture("writeColor", sun_tex_id);
	graphics->bindTexture("cloudsTex", clouds_tex_id);

	graphics->dispatch(param);

	graphics->bindShader(invalid_index);
}

void CComputeSkySystem::computeComposition(const glm::mat4& projection, const glm::mat4& view)
{
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);

	FDispatchParam param{};
	param.size = { extent.width, extent.height, 1 };

	auto& lightsManager = EGEngine->getLights();
	auto& directional = lightsManager->getDirectional();

	auto invView = glm::inverse(view);
	auto invProjection = glm::inverse(projection);
	auto invViewProjection = glm::inverse(projection * view);

	graphics->bindShader(composition_shader_id);

	auto& pCamera = graphics->getUniformHandle("FCameraData");
	pCamera->set("view", view);
	pCamera->set("invView", invView);
	pCamera->set("projection", projection);
	pCamera->set("invProjection", invProjection);
	pCamera->set("invViewProjection", invViewProjection);

	//auto& pScattering = graphics->getUniformHandle("FAtmosphereScattering");
	//pScattering->set("lightCount", directional.light_count);
	//
	//auto& pLights = graphics->getUniformHandle("UBOLights");
	//pLights->set("directionalLights", directional.lights);

	graphics->bindTexture("writeColor", composition_tex_id);
	graphics->bindTexture("atmosphereTex", atmosphere_tex_id);
	graphics->bindTexture("cloudsTex", clouds_tex_id);
	graphics->bindTexture("cloudsNoiseHighSampler", perlin_noise_tex_id);
	graphics->bindTexture("cloudsNoiseLowSampler", cellular_noise_tex_id);

	graphics->dispatch(param);

	graphics->bindShader(invalid_index);
}