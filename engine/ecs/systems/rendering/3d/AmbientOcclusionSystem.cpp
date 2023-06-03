#include "AmbientOcclusionSystem.h"
#include "image/Image.h"
#include "Engine.h"

#include "image/Image2D.h"
#include "graphics/GraphicsSettings.h"

#include "ecs/components/CameraComponent.h"

#include "Helpers.h"

#include <random>

using namespace engine::ecs;
using namespace engine::graphics;

void CAmbientOcclusionSystem::__create()
{
	std::default_random_engine rndEngine(0);
	std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

	{
		FShaderSpecials specials;
		specials.defines = 
		{
			{"SSAO_KERNEL_SIZE", std::to_string(SSAO_KERNEL_SIZE)}
		};
		ssao_shader_id = graphics->addShader("ssao_pass", specials);
	}

	gaussian_blur_shader_id = graphics->addShader("ambient_occlusion_blur");
	

	for (uint32_t i = 0; i < SSAO_KERNEL_SIZE; ++i)
	{
		glm::vec3 sample(rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine));
		sample = glm::normalize(sample);
		sample *= rndDist(rndEngine);
		float scale = float(i) / float(SSAO_KERNEL_SIZE);
		scale = lerp(0.1f, 1.0f, scale * scale);
		vSSAOKernel[i] = glm::vec4(sample * scale, 0.0f);
	}

	std::array<glm::vec4, SSAO_NOISE_DIM* SSAO_NOISE_DIM> vSSAONoise;
	for (uint32_t i = 0; i < static_cast<uint32_t>(vSSAONoise.size()); i++)
		vSSAONoise[i] = glm::vec4(rndDist(rndEngine) * 2.0f - 1.0f, rndDist(rndEngine) * 2.0f - 1.0f, 0.0f, 0.0f);

	// Create noise image
	auto pNoise = std::make_unique<CImage>(graphics->getDevice().get());
	pNoise->create(vSSAONoise.data(), vk::Extent3D{ SSAO_NOISE_DIM, SSAO_NOISE_DIM, sizeof(glm::vec4) }, vk::Format::eR32G32B32A32Sfloat);

	noise_image = graphics->addImage("ssao_noise_image", std::move(pNoise));

	addSubresource("depth_tex");
	addSubresource("raw_ao_tex");

	CBaseGraphicsSystem::__create();
}

void CAmbientOcclusionSystem::__update(float fDt)
{
	auto& settings = CGraphicsSettings::getInstance()->getSettings();
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);

	auto* camera = EGEngine->getActiveCamera();

	if (!camera || !settings.bEnableAmbientOcclusion)
		return;

	auto projection = camera->projection;
	projection[1][1] *= -1.f;

	auto stage = graphics->getRenderStageID("ssao");

	graphics->bindShader(ssao_shader_id);

	graphics->bindTexture("depth_tex", getSubresource("depth_tex"));
	graphics->bindTexture("ssao_noise_tex", noise_image);

	auto& pUBO = graphics->getUniformHandle("UBOAmbientOcclusion");
	pUBO->set("samples", vSSAOKernel);
	pUBO->set("invProjection", glm::inverse(projection));
	pUBO->set("projection", projection);
	pUBO->set("zNear", camera->nearPlane);
	pUBO->set("zFar", camera->farPlane);
	pUBO->set("radius", settings.fAmbientOcclusionRadius);
	pUBO->set("bias", settings.fAmbientOcclusionBias);
		
	graphics->bindRenderer(stage);
	graphics->flushShader();
	graphics->draw(0, 3, 0, 0, 1);
	graphics->bindRenderer(invalid_index);

	// Blur pass
	updateOutputImage("ambient_occlusion_tex", final_ao_image);

	graphics->BarrierFromGraphicsToCompute(getSubresource("raw_ao_tex"));
	graphics->BarrierFromGraphicsToCompute(getSubresource("depth_tex"));

	graphics->bindShader(gaussian_blur_shader_id);

	auto& pPush = graphics->getPushBlockHandle("ubo");
	pPush->set("blur_range", settings.iAmbientOcclusionSmoothingSteps);

	FDispatchParam param;
	param.size = { extent.width, extent.height, 1.f };

	graphics->bindTexture("writeColor", final_ao_image);
	graphics->bindTexture("samplerColor", getSubresource("raw_ao_tex"));

	graphics->dispatch(param);

	graphics->bindShader(invalid_index);

	graphics->BarrierFromComputeToGraphics();
}

void CAmbientOcclusionSystem::updateOutputImage(const std::string& image_name, size_t& image_id)
{
	auto& device = graphics->getDevice();
	auto& image = graphics->getImage(image_id);
	auto extent = device->getExtent(true);

	bool recreateFlag{ true };
	if (image)
	{
		auto img_extent = image->getExtent();
		recreateFlag = extent.width != img_extent.width || extent.height != img_extent.height;
	}

	if (recreateFlag)
	{
		graphics->removeImage(image_id);

		auto pImage = std::make_unique<CImage2D>(device.get());
		pImage->create(
			extent,
			vk::Format::eR8Unorm,
			vk::ImageLayout::eGeneral,
			vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
			vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment |
			vk::ImageUsageFlagBits::eStorage,
			vk::ImageAspectFlagBits::eColor,
			vk::Filter::eLinear,
			vk::SamplerAddressMode::eClampToEdge,
			vk::SampleCountFlagBits::e1, true, false);

		image_id = graphics->addImage(image_name, std::move(pImage));
	}
}