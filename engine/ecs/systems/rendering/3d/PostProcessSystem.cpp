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

CPostProcessSystem::~CPostProcessSystem()
{
	
}

void CPostProcessSystem::__create()
{
	shader_id_tonemap = EGGraphics->addShader("post_process", "post_process");
	shader_id_downsample = EGGraphics->addShader("downsample", "downsample");
	shader_id_blur = EGGraphics->addShader("gaussian_blur", "gaussian_blur");

	bloom_image = createImage("bloom_tex", vk::Format::eR16G16B16A16Sfloat);

	addSubresource("composition_tex");
	addSubresource("brightness_tex");

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
		auto& image = EGGraphics->getImage(bloom_image);
		auto image_ext = image->getExtent();
		if (image_ext.width != extent.width || image_ext.height != extent.height)
		{
			EGGraphics->removeImage(bloom_image);
			bloom_image = createImage("bloom_tex", vk::Format::eR16G16B16A16Sfloat);
		}
	}

	// Apply fxaa (graphics) + brightdetect
	// Downsample
	// blur vert
	// blur horiz


	// Tonemapping
	{
		auto& stage = EGGraphics->getRenderStage("tonemapping");
		auto& pShader = EGGraphics->getShader(shader_id_tonemap);
		
		pShader->addTexture("samplerColor", getSubresource("composition_tex"));

		auto& pPush = pShader->getPushBlock("ubo");
		pPush->set("enableFXAA", peffects.fxaa);
		pPush->set("texelStep", glm::vec2(1.f / extent.width, 1.f / extent.height));
		pPush->set("bloom_threshold", peffects.bloom_threshold);
		pPush->flush(commandBuffer);

		auto& pUTonemap = pShader->getUniformBuffer("UBOTonemap");
		pUTonemap->set("gamma", peffects.gamma);
		pUTonemap->set("exposure", peffects.exposure);

		auto& pUFXAA = pShader->getUniformBuffer("UBOFXAA");
		pUFXAA->set("qualitySubpix", peffects.qualitySubpix);
		pUFXAA->set("qualityEdgeThreshold", peffects.qualityEdgeThreshold);
		pUFXAA->set("qualityEdgeThresholdMin", peffects.qualityEdgeThresholdMin);

		stage->begin(commandBuffer);
		pShader->predraw(commandBuffer);
		commandBuffer.draw(3, 1, 0, 0);

		stage->end(commandBuffer);
	}

	VkHelper::BarrierFromGraphicsToCompute(commandBuffer, getSubresource("brightness_tex"));

	// Downsample pass
	{
		auto& pShader = EGGraphics->getShader(shader_id_downsample);
	
		pShader->addTexture("writeColour", bloom_image);
		pShader->addTexture("samplerColour", getSubresource("brightness_tex"));
		
		pShader->dispatch(commandBuffer, device->getExtent(true));
	}

	VkHelper::BarrierFromComputeToGraphics(commandBuffer);
}