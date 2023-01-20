#include "PostProcessSystem.h"
#include "Engine.h"

#include "graphics/image/Image2D.h"

#ifdef MemoryBarrier
#undef MemoryBarrier
#endif

using namespace engine::graphics;
using namespace engine::ecs;
using namespace engine::system::window;

size_t createImage(const std::string& name, vk::Format format, bool mips = false)
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
		vk::ImageUsageFlagBits::eStorage,
		vk::ImageAspectFlagBits::eColor,
		vk::Filter::eLinear,
		vk::SamplerAddressMode::eClampToEdge,
		vk::SampleCountFlagBits::e1, true, false, mips);

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
	final_image = createImage("final_tex", vk::Format::eR32G32B32A32Sfloat);
	brightness_image = createImage("brightness_tex", vk::Format::eR32G32B32A32Sfloat);
	temp_image = createImage("bloom_tex", vk::Format::eR32G32B32A32Sfloat, true);
	temp_image_2 = createImage("bloom_tex_2", vk::Format::eR32G32B32A32Sfloat, true);
	auto& image = EGGraphics->getImage(temp_image);

	FShaderSpecials specials;
	specials.usages = image->getMipLevels();
	shader_downsample = EGGraphics->addShader("downsample", "downsample", specials);
	shader_upsample = EGGraphics->addShader("upsample", "upsample", specials);

	shader_fxaa = EGGraphics->addShader("fxaa", "fxaa");
	shader_brightdetect = EGGraphics->addShader("brightdetect", "brightdetect");
	shader_tonemap = EGGraphics->addShader("tonemap", "tonemap");

	
	specials.usages = 2;
	shader_blur = EGGraphics->addShader("gaussian_blur", "gaussian_blur", specials);

	

	addSubresource("composition_tex");

	CBaseGraphicsSystem::__create();
}

void CPostProcessSystem::__update(float fDt)
{
	auto& device = EGGraphics->getDevice();
	auto extent = device->getExtent(true);
	auto resolution = glm::vec2(static_cast<float>(extent.width), static_cast<float>(extent.height));
	auto& peffects = EGEngine->getPostEffects();
	auto commandBuffer = EGGraphics->getCommandBuffer();

	// Re create images
	{
		tryReCreateImage("final_tex", final_image);
		tryReCreateImage("brightness_tex", brightness_image);
		tryReCreateImage("bloom_tex", temp_image);
		tryReCreateImage("bloom_tex_2", temp_image_2);
	}

	{
		for (auto& view : vDeleteViews)
			device->destroy(&view);
		vDeleteViews.clear();
	}

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
		
		pShader->dispatch(commandBuffer, resolution);

	}

	VkHelper::BarrierFromComputeToCompute(commandBuffer);

	// Brightdetect
	{
		auto& pShader = EGGraphics->getShader(shader_brightdetect);

		auto& pPush = pShader->getPushBlock("ubo");
		pPush->set("bloom_threshold", peffects.bloom_threshold);
		pPush->flush(commandBuffer);

		pShader->addTexture("writeColor", brightness_image);
		pShader->addTexture("samplerColor", final_image);

		pShader->dispatch(commandBuffer, resolution);
	}

	VkHelper::BarrierFromComputeToCompute(commandBuffer);
	
	// Downsample pass
	{
		auto& pShader = EGGraphics->getShader(shader_downsample);
		auto& image = EGGraphics->getImage(temp_image);
		auto& pPush = pShader->getPushBlock("ubo");

		glm::vec2 mipSize{ extent.width, extent.height };

		for (uint32_t i = 0; i < image->getMipLevels(); i++)
		{
			vk::ImageView levelView{ VK_NULL_HANDLE };
			vk::ImageViewCreateInfo viewInfo{};
			viewInfo.viewType = vk::ImageViewType::e2D;
			viewInfo.format = image->getFormat();
			viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			viewInfo.subresourceRange.baseMipLevel = i;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;
			viewInfo.image = image->getImage();

			vk::Result res = device->create(viewInfo, &levelView);
			assert(res == vk::Result::eSuccess && "Cannot create image view.");

			pPush->set("resolution", mipSize);
			pPush->flush(commandBuffer);

			auto imageInfo = image->getDescriptor();
			imageInfo.imageView = levelView;

			pShader->addTexture("writeColor", imageInfo);
			pShader->addTexture("samplerColor", brightness_image);

			pShader->dispatch(commandBuffer, { mipSize.x, mipSize.y });

			VkHelper::BarrierFromComputeToCompute(commandBuffer);

			vDeleteViews.emplace_back(levelView);

			mipSize *= 0.5f;
		}
	}

	//VkHelper::BarrierFromComputeToCompute(commandBuffer);
	
	// Upsample
	{
		auto& pShader = EGGraphics->getShader(shader_upsample);
		auto& image = EGGraphics->getImage(temp_image_2);
		auto& pPush = pShader->getPushBlock("ubo");
		pPush->set("filterRadius", peffects.filterRadius);

		glm::vec2 mipSize{ extent.width, extent.height };

		for (uint32_t i = 0; i < image->getMipLevels(); i++)
		{
			vk::ImageView levelView{ VK_NULL_HANDLE };
			vk::ImageViewCreateInfo viewInfo{};
			viewInfo.viewType = vk::ImageViewType::e2D;
			viewInfo.format = image->getFormat();
			viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			viewInfo.subresourceRange.baseMipLevel = i;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;
			viewInfo.image = image->getImage();

			vk::Result res = device->create(viewInfo, &levelView);
			assert(res == vk::Result::eSuccess && "Cannot create image view.");

			pPush->flush(commandBuffer);

			auto imageInfo = image->getDescriptor();
			imageInfo.imageView = levelView;

			pShader->addTexture("writeColor", imageInfo);
			pShader->addTexture("samplerColor", temp_image);

			pShader->dispatch(commandBuffer, { mipSize.x, mipSize.y });

			VkHelper::BarrierFromComputeToCompute(commandBuffer);

			vDeleteViews.emplace_back(levelView);

			mipSize *= 0.5f;
		}
	}
	
	//VkHelper::BarrierFromComputeToCompute(commandBuffer);
	
	// Tonemap
	{
		auto& pShader = EGGraphics->getShader(shader_tonemap);
	
		auto& pBlock = pShader->getPushBlock("ubo");
		pBlock->set("gamma", peffects.gamma);
		pBlock->set("exposure", peffects.exposure);
		pBlock->set("bloomStrength", peffects.bloomStrength);
		pBlock->flush(commandBuffer);
	
		pShader->addTexture("writeColor", temp_image);
		pShader->addTexture("samplerColor", final_image);
		pShader->addTexture("blurColor", temp_image_2);
	
		pShader->dispatch(commandBuffer, resolution);
	}

	VkHelper::BarrierFromComputeToGraphics(commandBuffer);
}