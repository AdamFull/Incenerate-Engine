#include "BloomEffect.h"

#include "Engine.h"
#include "EffectShared.h"

#include "ecs/components/CameraComponent.h"

using namespace engine::graphics;
using namespace engine::ecs;

CBloomEffect::~CBloomEffect()
{
	auto& graphics = EGGraphics;
	
	graphics->removeShader(shader_brightdetect);
	graphics->removeShader(shader_downsample);
	graphics->removeShader(shader_upsample);
	graphics->removeShader(shader_applybloom);

	graphics->removeImage(bloom_image);
	graphics->removeImage(final_image);
}

void CBloomEffect::create()
{
	init();
	auto& image = EGGraphics->getImage(bloom_image);

	FShaderSpecials specials;
	specials.usages = mipLevels;
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
	auto& graphics = EGGraphics;
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);

	graphics->removeImage(final_image);
	graphics->removeImage(bloom_image);

	final_image = effectshared::createImage("final_bloom_tex", vk::Format::eB10G11R11UfloatPack32);
	bloom_image = effectshared::createImage("bloom_stage_tex", vk::Format::eB10G11R11UfloatPack32, true, mipLevels);

	vMips.clear();

	glm::vec2 mipLevel{ extent.width, extent.height };

	for (uint32_t i = 0; i < mipLevels; i++)
	{
		vMips.emplace_back(mipLevel);
		mipLevel *= 0.5f;
	}
}

size_t CBloomEffect::render(FCameraComponent* camera, size_t source)
{
	auto& graphics = EGGraphics;
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);
	auto resolution = glm::vec2(static_cast<float>(extent.width), static_cast<float>(extent.height));

	if (camera->effects.bloom.enable)
	{
		// Brightdetect
		{
			graphics->bindShader(shader_brightdetect);

			auto& pPush = graphics->getPushBlockHandle("ubo");
			pPush->set("bloom_threshold", camera->effects.bloom.threshold);

			graphics->bindTexture("writeColor", final_image);
			graphics->bindTexture("samplerColor", source);

			graphics->dispatch(resolution);

			graphics->bindShader(invalid_index);
		}

		VkHelper::BarrierFromComputeToCompute();

		// Downsample
		{
			graphics->bindShader(shader_downsample);
			auto& pPush = graphics->getPushBlockHandle("ubo");

			graphics->bindTexture("samplerColor", final_image);

			for (uint32_t i = 0; i < mipLevels; i++)
			{
				auto& mip = vMips.at(i);

				pPush->set("resolution", mip);
				pPush->set("mipLevel", i);

				graphics->bindTexture("writeColor", bloom_image, i);
				graphics->dispatch(mip);

				VkHelper::BarrierFromComputeToCompute();

				graphics->bindTexture("samplerColor", bloom_image, i);
			}

			graphics->bindShader(invalid_index);
		}		

		// Upsample
		{
			graphics->bindShader(shader_upsample);
			auto& pPush = graphics->getPushBlockHandle("ubo");

			for (uint32_t i = mipLevels - 1; i > 0; i--)
			{
				auto& mip = vMips.at(i);
				auto& nextMip = vMips.at(i - 1);

				pPush->set("resolution", nextMip);
				pPush->set("filter_radius", camera->effects.bloom.filter_radius);

				graphics->bindTexture("samplerColor", bloom_image, i);
				graphics->bindTexture("writeColor", bloom_image, i - 1);

				graphics->dispatch(nextMip);

				VkHelper::BarrierFromComputeToCompute();
			}

			graphics->bindShader(invalid_index);
		}

		// Apply bloom
		{
			graphics->bindShader(shader_applybloom);
			auto& pPush = graphics->getPushBlockHandle("ubo");
			pPush->set("bloom_strength", camera->effects.bloom.strength);

			graphics->bindTexture("writeColor", final_image);
			graphics->bindTexture("samplerColor", source);
			graphics->bindTexture("bloomColor", bloom_image);

			graphics->dispatch(resolution);

			graphics->bindShader(invalid_index);

			VkHelper::BarrierFromComputeToCompute();
		}

		return final_image;
	}
	

	return source;
}