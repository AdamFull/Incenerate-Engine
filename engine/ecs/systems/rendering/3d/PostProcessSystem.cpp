#include "PostProcessSystem.h"
#include "Engine.h"

#include "postprocess/EffectShared.h"

#include "graphics/GraphicsSettings.h"

#include "ecs/components/CameraComponent.h"
#include "ecs/helper.hpp"

using namespace engine::graphics;
using namespace engine::ecs;

bool try_recreate_texture(CAPIHandle* graphics, size_t& texture_id, const std::string& name, vk::Format format, uint32_t mipLevels = 1)
{
	auto& device = graphics->getDevice();
	auto& image = graphics->getImage(texture_id);
	if (!image)
	{
		texture_id = effectshared::createImage(name, format);
		return true;
	}

	auto extent = device->getExtent(true);
	auto img_extent = image->getExtent();

	if (extent.width != img_extent.width || extent.height != img_extent.height)
	{
		graphics->removeImage(texture_id);
		texture_id = effectshared::createImage(name, format, mipLevels > 1, mipLevels);
		return true;
	}

	return false;
}

CPostProcessSystem::~CPostProcessSystem()
{
	
}

void CPostProcessSystem::__create()
{
	FShaderCreateInfo specials;
	specials.bind_point = vk::PipelineBindPoint::eCompute;
	specials.usages = 1;

	shader = graphics->addShader("postprocess:high_pass", specials);
	shader_blur = graphics->addShader("postprocess:bokeh", specials);
	shader_avg_brightness = graphics->addShader("postprocess:avg_brightness", specials);

	avg_lum_image = effectshared::createImage("avg_brightness_image", vk::Format::eR32Sfloat);

	FShaderCreateInfo upsample_downsample_ci;
	upsample_downsample_ci.bind_point = vk::PipelineBindPoint::eCompute;
	upsample_downsample_ci.usages = bloomMipLevels;

	shader_downsample = graphics->addShader("postprocess:downsample", upsample_downsample_ci);
	shader_upsample = graphics->addShader("postprocess:upsample", upsample_downsample_ci);

	addSubresource("composition_tex");
	addSubresource("brightcolor_tex");
	addSubresource("global_illumination_tex");
	addSubresource("reflections_tex");
	
	addSubresource("depth_tex");

	CBaseGraphicsSystem::__create();
}

void CPostProcessSystem::__update(float fDt)
{
	auto& settings = CGraphicsSettings::getInstance()->getSettings();

	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);

	FDispatchParam param;
	param.size = { extent.width, extent.height, 1.f };

	auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	update_images();

	size_t current_image{ invalid_index };
	if (settings.bEnableGlobalIllumination)
		current_image = settings.bEnableReflections ? getSubresource("reflections_tex") : getSubresource("global_illumination_tex");
	else
		current_image = settings.bEnableReflections ? getSubresource("reflections_tex") : getSubresource("composition_tex");

	graphics->BarrierFromGraphicsToCompute(current_image);

	auto depth_image = getSubresource("depth_tex");
	auto bright_image = getSubresource("brightcolor_tex");
	graphics->BarrierFromGraphicsToCompute(depth_image);

	if (camera->effects.tonemap.enable)
	{
		std::vector<FDispatchParam> params;
		params.resize(2);

		// Histogram
		params[0].size = { extent.width, extent.height, 1.f };
		// Average
		params[1].size = { 1.f, 1.f, 1.f };
		params[1].divideByLocalSizes = false;

		graphics->bindShader(shader_avg_brightness);

		float logLumRange = camera->effects.tonemap.lumMax - camera->effects.tonemap.lumMin;
		float timeCoeff = glm::clamp<float>(1.0f - glm::exp(-fDt * camera->effects.tonemap.tau), 0.0, 1.0);

		auto& pParams = graphics->getUniformHandle("UBOParams");
		pParams->set("minLogLum", camera->effects.tonemap.lumMin);
		pParams->set("logLumRange", logLumRange);
		pParams->set("timeCoeff", timeCoeff);
		pParams->set("numPixelsX", static_cast<float>(extent.width));
		pParams->set("numPixelsY", static_cast<float>(extent.height));

		graphics->bindTexture("samplerColor", current_image);
		graphics->bindTexture("target", avg_lum_image);

		graphics->dispatch(params);

		graphics->bindShader(invalid_index);

		graphics->BarrierFromComputeToCompute();
	}

	if (camera->effects.dof.enable)
	{
		graphics->bindShader(shader_blur);

		auto& pPush = graphics->getPushBlockHandle("ubo");
		pPush->set("max_radius", camera->effects.dof.bokeh_samples);
		pPush->set("sides", camera->effects.dof.bokeh_poly);

		graphics->bindTexture("writeColor", blurred_image);
		graphics->bindTexture("samplerColor", current_image);

		graphics->dispatch(param);

		graphics->bindShader(invalid_index);

		// TODO: may be use timeline semaphore here?
		graphics->BarrierFromComputeToCompute();
	}

	if (camera->effects.bloom.enable)
	{
		// Downsample
		{
			graphics->bindShader(shader_downsample);
			auto& pPush = graphics->getPushBlockHandle("ubo");

			graphics->bindTexture("samplerColor", bright_image);

			for (uint32_t i = 0; i < bloomMipLevels; i++)
			{
				auto& mip = vMips.at(i);
				FDispatchParam mip_param;
				mip_param.size = mip;

				pPush->set("resolution", mip);
				pPush->set("mipLevel", i);

				graphics->bindTexture("writeColor", bloom_image, i);
				graphics->dispatch(mip_param);

				graphics->BarrierFromComputeToCompute();

				graphics->bindTexture("samplerColor", bloom_image, i);
			}

			graphics->bindShader(invalid_index);
		}

		// Upsample
		{
			graphics->bindShader(shader_upsample);
			auto& pPush = graphics->getPushBlockHandle("ubo");

			for (uint32_t i = bloomMipLevels - 1; i > 0; i--)
			{
				auto& mip = vMips.at(i);
				auto& nextMip = vMips.at(i - 1);

				pPush->set("resolution", nextMip);
				pPush->set("filter_radius", camera->effects.bloom.filter_radius);

				graphics->bindTexture("samplerColor", bloom_image, i);
				graphics->bindTexture("writeColor", bloom_image, i - 1);

				graphics->dispatch(param);

				graphics->BarrierFromComputeToCompute();
			}

			graphics->bindShader(invalid_index);
		}
	}

	graphics->bindShader(shader);

	graphics->bindTexture("writeColor", final_image);
	graphics->bindTexture("samplerColor", current_image);
	graphics->bindTexture("samplerDepth", depth_image);
	graphics->bindTexture("samplerBlured", blurred_image);
	graphics->bindTexture("bloomColor", bloom_image);
	graphics->bindTexture("avgLuminosity", avg_lum_image);

	auto& pPush = graphics->getUniformHandle("UBOHighPassEffects");
	pPush->set("camera_pos", camera->viewPos);
	pPush->set("inv_view_projection", glm::inverse(camera->projection* camera->view));

	// FXAA uniforms
	pPush->set("enable_fxaa", static_cast<int32_t>(camera->effects.fxaa.enable));
	pPush->set("aa_quality", atof(fxaa_quality_variant[camera->effects.fxaa.quality]));
	pPush->set("aa_threshold", atof(fxaa_threshold_variant[camera->effects.fxaa.threshold]));
	pPush->set("aa_threshold_min", atof(fxaa_threshold_min_variant[camera->effects.fxaa.threshold_min]));

	// Tonemap uniforms
	pPush->set("enable_tonemap", static_cast<int32_t>(camera->effects.tonemap.enable));
	pPush->set("tm_gamma", camera->effects.tonemap.gamma);
	pPush->set("tm_exposure", camera->effects.tonemap.exposure);
	pPush->set("tm_white_point", camera->effects.tonemap.whitePoint);
	pPush->set("tm_adaptive", static_cast<int32_t>(camera->effects.tonemap.adaptive));

	// bloom uniforms
	pPush->set("enable_bloom", static_cast<int32_t>(camera->effects.bloom.enable));
	pPush->set("b_bloom_strength", camera->effects.bloom.strength);

	// Depth of field uniforms
	pPush->set("enable_dof", static_cast<int32_t>(camera->effects.dof.enable));
	pPush->set("df_near_plane", camera->nearPlane);
	pPush->set("df_far_plane", camera->farPlane);

	pPush->set("df_focus_point", camera->effects.dof.focus_point);
	pPush->set("df_near_field", camera->effects.dof.near_field);
	pPush->set("df_near_transition", camera->effects.dof.near_transition);
	pPush->set("df_far_field", camera->effects.dof.far_field);
	pPush->set("df_far_transition", camera->effects.dof.far_transition);

	// Chromatic aberration uniforms
	pPush->set("enable_chromatic_aberration", static_cast<int32_t>(camera->effects.aberration.enable));
	pPush->set("ca_distortion", camera->effects.aberration.distortion);
	pPush->set("ca_iterations", camera->effects.aberration.iterations);

	// Fog uniforms
	pPush->set("enable_fog", static_cast<int32_t>(camera->effects.fog.enable));
	pPush->set("fog_color", camera->effects.fog.color);
	pPush->set("fog_density", camera->effects.fog.density);
	pPush->set("fog_max", camera->farPlane);

	// Finm grain uniforms
	pPush->set("enable_film_grain", static_cast<int32_t>(camera->effects.filmgrain.enable));
	pPush->set("fg_amount", camera->effects.filmgrain.amount);
	pPush->set("fg_time", fDt);

	// Vignette uniforms
	pPush->set("enable_vignette", static_cast<int32_t>(camera->effects.vignette.enable));
	pPush->set("v_inner_radius", camera->effects.vignette.inner);
	pPush->set("v_outer_radius", camera->effects.vignette.outer);
	pPush->set("v_opacity", camera->effects.vignette.opacity);

	graphics->dispatch(param);

	graphics->bindShader(invalid_index);

	graphics->BarrierFromComputeToGraphics();
}

void CPostProcessSystem::update_images()
{
	try_recreate_texture(graphics, final_image, "postprocess_tex", vk::Format::eB10G11R11UfloatPack32);
	try_recreate_texture(graphics, blurred_image, "dof_blur_tex", vk::Format::eB10G11R11UfloatPack32);
	if (try_recreate_texture(graphics, bloom_image, "bloom_stage_tex", vk::Format::eB10G11R11UfloatPack32, bloomMipLevels))
	{
		auto& device = graphics->getDevice();
		auto extent = device->getExtent(true);
		glm::vec3 mipLevel{ extent.width, extent.height, 1.f };

		vMips.clear();
		for (uint32_t i = 0; i < bloomMipLevels; i++)
		{
			vMips.emplace_back(mipLevel);
			mipLevel *= 0.5f;
		}
	}
}