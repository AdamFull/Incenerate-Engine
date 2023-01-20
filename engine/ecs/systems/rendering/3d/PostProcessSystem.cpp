#include "PostProcessSystem.h"
#include "Engine.h"

#include "graphics/image/Image2D.h"

#ifdef MemoryBarrier
#undef MemoryBarrier
#endif

using namespace engine::graphics;
using namespace engine::ecs;
using namespace engine::system::window;

size_t createImage(const std::string& name, vk::Format format)
{
	using namespace engine;

	auto& device = EGGraphics->getDevice();

	auto pImage = std::make_unique<CImage2D>(device.get());
	pImage->create(
		device->getExtent(true),
		format,
		vk::ImageLayout::eGeneral,
		vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
		vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment |
		vk::ImageUsageFlagBits::eStorage);

	return EGGraphics->addImage(name, std::move(pImage));
}

void tryReCreateImage(const std::string& name, size_t& image_id)
{
	using namespace engine;

	auto& device = EGGraphics->getDevice();
	auto extent = device->getExtent(true);

	auto& image = EGGraphics->getImage(image_id);
	auto image_ext = image->getExtent();
	if (image_ext.width != extent.width || image_ext.height != extent.height)
	{
		EGGraphics->removeImage(image_id);
		image_id = createImage(name, vk::Format::eR16G16B16A16Sfloat);
	}
}

CPostProcessSystem::~CPostProcessSystem()
{
	
}

void CPostProcessSystem::__create()
{
	shader_fxaa = EGGraphics->addShader("fxaa", "fxaa");
	shader_brightdetect = EGGraphics->addShader("brightdetect", "brightdetect");
	shader_downsample = EGGraphics->addShader("downsample", "downsample");
	shader_tonemap = EGGraphics->addShader("tonemap", "tonemap");

	FShaderSpecials specials;
	specials.usages = 2;
	shader_blur = EGGraphics->addShader("gaussian_blur", "gaussian_blur", specials);

	final_image = createImage("final_tex", vk::Format::eR32G32B32A32Sfloat);
	temp_image = createImage("bloom_tex", vk::Format::eR32G32B32A32Sfloat);
	temp_image_2 = createImage("bloom_tex_2", vk::Format::eR32G32B32A32Sfloat);

	addSubresource("composition_tex");

	CBaseGraphicsSystem::__create();
}

void CPostProcessSystem::__update(float fDt)
{
	auto& device = EGGraphics->getDevice();
	auto extent = device->getExtent(true);
	auto& peffects = EGEngine->getPostEffects();
	auto commandBuffer = EGGraphics->getCommandBuffer();

	// Re create images
	{
		tryReCreateImage("final_tex", final_image);
		tryReCreateImage("bloom_tex", temp_image);
		tryReCreateImage("bloom_tex_2", temp_image_2);
	}

	// Apply fxaa (graphics) + brightdetect
	// Downsample
	// blur vert
	// blur horiz

	VkHelper::BarrierFromGraphicsToCompute(commandBuffer, getSubresource("composition_tex"));

	// FXAA
	{
		auto& pShader = EGGraphics->getShader(shader_fxaa);

		pShader->addTexture("writeColor", final_image);
		pShader->addTexture("samplerColor", getSubresource("composition_tex"));

		auto& pPush = pShader->getPushBlock("ubo");
		pPush->set("texelStep", glm::vec2(1.f / extent.width, 1.f / extent.height));
		pPush->set("qualitySubpix", peffects.qualitySubpix);
		pPush->set("qualityEdgeThreshold", peffects.qualityEdgeThreshold);
		pPush->set("qualityEdgeThresholdMin", peffects.qualityEdgeThresholdMin);
		pPush->flush(commandBuffer);
		
		pShader->dispatch(commandBuffer, device->getExtent(true));

	}

	VkHelper::BarrierFromComputeToCompute(commandBuffer);

	// Brightdetect
	{
		auto& pShader = EGGraphics->getShader(shader_brightdetect);

		auto& pPush = pShader->getPushBlock("ubo");
		pPush->set("bloom_threshold", peffects.bloom_threshold);
		pPush->flush(commandBuffer);

		pShader->addTexture("writeColor", temp_image);
		pShader->addTexture("samplerColor", final_image);

		pShader->dispatch(commandBuffer, device->getExtent(true));
	}

	//VkHelper::BarrierFromComputeToCompute(commandBuffer);
	//
	//// Downsample pass
	//{
	//	auto& pShader = EGGraphics->getShader(shader_downsample);
	//
	//	pShader->addTexture("writeColor", temp_image_2);
	//	pShader->addTexture("samplerColor", temp_image);
	//	
	//	pShader->dispatch(commandBuffer, device->getExtent(true));
	//}

	VkHelper::BarrierFromComputeToCompute(commandBuffer);

	{
		auto& pShader = EGGraphics->getShader(shader_blur);

		auto& pBlock = pShader->getPushBlock("ubo");
		pBlock->set("blurScale", peffects.blurScale);
		pBlock->set("blurStrength", peffects.blurStrength);
		pBlock->set("direction", -1);
		pBlock->flush(commandBuffer);

		pShader->addTexture("writeColor", temp_image_2);
		pShader->addTexture("samplerColor", temp_image);

		pShader->dispatch(commandBuffer, device->getExtent(true));

		VkHelper::BarrierFromComputeToCompute(commandBuffer);

		pShader->addTexture("writeColor", temp_image);
		pShader->addTexture("samplerColor", temp_image_2);
		pBlock->set("direction", 1);
		pBlock->flush(commandBuffer);

		pShader->dispatch(commandBuffer, device->getExtent(true));
	}

	VkHelper::BarrierFromComputeToCompute(commandBuffer);

	// Tonemap
	{
		auto& pShader = EGGraphics->getShader(shader_tonemap);

		auto& pBlock = pShader->getPushBlock("ubo");
		pBlock->set("gamma", peffects.gamma);
		pBlock->set("exposure", peffects.exposure);
		pBlock->flush(commandBuffer);

		pShader->addTexture("writeColor", temp_image_2);
		pShader->addTexture("samplerColor", final_image);
		pShader->addTexture("blurColor", temp_image);

		pShader->dispatch(commandBuffer, device->getExtent(true));
	}

	VkHelper::BarrierFromComputeToGraphics(commandBuffer);
}