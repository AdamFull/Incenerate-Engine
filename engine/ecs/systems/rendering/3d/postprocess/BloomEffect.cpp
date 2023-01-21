#include "BloomEffect.h"

#include "Engine.h"
#include "EffectShared.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CBloomEffect::create()
{
	init();
	auto& image = EGGraphics->getImage(bloom_image);

	FShaderSpecials specials;
	specials.usages = image->getMipLevels();
	shader_downsample = EGGraphics->addShader("downsample", "downsample", specials);
	shader_upsample = EGGraphics->addShader("upsample", "upsample", specials);
	shader_brightdetect = EGGraphics->addShader("brightdetect", "brightdetect");
	shader_applybloom = EGGraphics->addShader("applybloom", "applybloom");
}

void CBloomEffect::update()
{
	auto& device = EGGraphics->getDevice();
	auto& image = EGGraphics->getImage(bloom_image);
	auto extent = device->getExtent(true);
	auto img_extent = image->getExtent();

	if (extent.width != img_extent.width || extent.height != img_extent.height)
		init();
}

void CBloomEffect::init()
{
	auto& device = EGGraphics->getDevice();
	auto extent = device->getExtent(true);

	EGGraphics->removeImage(final_image);
	EGGraphics->removeImage(bloom_image);

	final_image = effectshared::createImage("final_bloom_tex", vk::Format::eB10G11R11UfloatPack32);
	bloom_image = effectshared::createImage("bloom_stage_tex", vk::Format::eB10G11R11UfloatPack32, true);

	for (auto& mip : vMips)
		device->destroy(&mip.view);
	vMips.clear();

	auto image = EGGraphics->getImage(bloom_image).get();

	glm::vec2 mipLevel{ extent.width, extent.height };

	for (uint32_t i = 0; i < image->getMipLevels(); i++)
	{
		FBloomMip bloomMip;

		vk::ImageViewCreateInfo viewInfo{};
		viewInfo.viewType = vk::ImageViewType::e2D;
		viewInfo.format = image->getFormat();
		viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		viewInfo.subresourceRange.baseMipLevel = i;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.image = image->getImage();

		vk::Result res = device->create(viewInfo, &bloomMip.view);
		assert(res == vk::Result::eSuccess && "Cannot create image view.");
		
		bloomMip.size = mipLevel;
		mipLevel *= 0.5f;
		
		vMips.emplace_back(bloomMip);
	}
}

size_t CBloomEffect::render(bool enable, size_t source)
{
	auto& device = EGGraphics->getDevice();
	auto extent = device->getExtent(true);
	auto resolution = glm::vec2(static_cast<float>(extent.width), static_cast<float>(extent.height));
	auto& peffects = EGEngine->getPostEffects();
	auto commandBuffer = EGGraphics->getCommandBuffer();

	if (enable)
	{
		// Brightdetect
		{
			auto& pShader = EGGraphics->getShader(shader_brightdetect);

			auto& pPush = pShader->getPushBlock("ubo");
			pPush->set("bloom_threshold", peffects.bloom_threshold);
			pPush->flush(commandBuffer);

			pShader->addTexture("writeColor", final_image);
			pShader->addTexture("samplerColor", source);

			pShader->dispatch(commandBuffer, resolution);
		}

		VkHelper::BarrierFromComputeToCompute(commandBuffer);

		// Downsample
		{
			auto& pShader = EGGraphics->getShader(shader_downsample);
			auto& image = EGGraphics->getImage(bloom_image);
			auto& pPush = pShader->getPushBlock("ubo");

			pShader->addTexture("samplerColor", final_image);

			for (uint32_t i = 0; i < image->getMipLevels(); i++)
			{
				auto& mip = vMips.at(i);

				pPush->set("resolution", mip.size);
				pPush->set("mipLevel", i);
				pPush->flush(commandBuffer);

				auto imageInfo = image->getDescriptor();
				imageInfo.imageView = mip.view;

				pShader->addTexture("writeColor", imageInfo);

				pShader->dispatch(commandBuffer, { mip.size.x, mip.size.y });

				VkHelper::BarrierFromComputeToCompute(commandBuffer);

				pShader->addTexture("samplerColor", imageInfo);
			}
		}

		// Upsample
		{
			auto& pShader = EGGraphics->getShader(shader_upsample);
			auto& image = EGGraphics->getImage(bloom_image);
			auto& pPush = pShader->getPushBlock("ubo");

			for (uint32_t i = image->getMipLevels() - 1; i > 0; i--)
			{
				auto& mip = vMips.at(i);
				auto& nextMip = vMips.at(i - 1);

				pPush->set("resolution", nextMip.size);
				pPush->set("filter_radius", peffects.bloom_filter_radius);
				pPush->flush(commandBuffer);

				auto imageInfo = image->getDescriptor();
				imageInfo.imageView = mip.view;
				pShader->addTexture("samplerColor", imageInfo);
				imageInfo.imageView = nextMip.view;
				pShader->addTexture("writeColor", imageInfo);

				pShader->dispatch(commandBuffer, { nextMip.size.x, nextMip.size.y });

				VkHelper::BarrierFromComputeToCompute(commandBuffer);
			}
		}

		// Apply bloom
		{
			auto& pShader = EGGraphics->getShader(shader_applybloom);
			auto& pPush = pShader->getPushBlock("ubo");
			pPush->set("bloom_strength", peffects.bloom_strength);
			pPush->flush(commandBuffer);

			pShader->addTexture("writeColor", final_image);
			pShader->addTexture("samplerColor", source);
			pShader->addTexture("bloomColor", bloom_image);

			pShader->dispatch(commandBuffer, resolution);

			VkHelper::BarrierFromComputeToCompute(commandBuffer);
		}

		return final_image;
	}
	

	return source;
}