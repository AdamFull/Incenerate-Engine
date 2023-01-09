#include "EnvironmentSystem.h"

#include "Engine.h"
#include "graphics/image/Image2D.h"
#include "graphics/image/ImageCubemap.h"

#include "ecs/components/SkyboxComponent.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/CameraComponent.h"

#include "ecs/helper.hpp"

using namespace engine::ecs;
using namespace engine::graphics;

void CEnvironmentSystem::__create()
{
	auto& registry = EGCoordinator;

	auto view = registry.view<FSkyboxComponent>();
	for (auto [entity, skybox] : view.each())
	{
		skybox.brdflut = computeBRDFLUT(512);
		skybox.irradiance = computeIrradiance(skybox.origin, 64);
		skybox.prefiltred = computePrefiltered(skybox.origin, 512);
	}
}

void CEnvironmentSystem::__update(float fDt)
{
	auto& registry = EGCoordinator;

	auto commandBuffer = EGGraphics->getCommandBuffer();

	auto ecamera = get_active_camera(registry);
	auto& camera = registry.get<FCameraComponent>(ecamera);
	auto& cameraTransform = registry.get<FTransformComponent>(ecamera);

	auto view = registry.view<FTransformComponent, FSkyboxComponent>();
	for (auto [entity, transform, skybox] : view.each())
	{
		auto& vbo = EGGraphics->getVertexBuffer(skybox.vbo_id);

		vbo->bind(commandBuffer);

		auto& pShader = EGGraphics->getShader(skybox.shader_id);
		if (pShader)
		{
			auto normal = glm::transpose(glm::inverse(transform.model));

			auto& pUBO = pShader->getUniformBuffer("FUniformData");
			pUBO->set("model", transform.model);
			pUBO->set("view", camera.view);
			pUBO->set("projection", camera.projection);
			pUBO->set("normal", normal);
			pUBO->set("viewDir", cameraTransform.rposition);
			pUBO->set("viewportDim", EGGraphics->getDevice()->getExtent());
			pUBO->set("frustumPlanes", camera.frustum.getFrustumSides());

			pShader->addTexture("samplerCubeMap", skybox.origin);

			pShader->predraw(commandBuffer);

			commandBuffer.drawIndexed(vbo->getLastIndex(), 1, 0, 0, 0);
		}
	}
}

size_t CEnvironmentSystem::computeBRDFLUT(uint32_t size)
{
	auto pDevice = EGGraphics->getDevice().get();
	auto brdfImage = std::make_unique<CImage2D>(pDevice);
	brdfImage->create(
		vk::Extent2D{ size, size }, 
		vk::Format::eR16G16Sfloat, 
		vk::ImageLayout::eGeneral,
		vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
		vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment |
		vk::ImageUsageFlagBits::eStorage);

	auto output_id = EGGraphics->addImage("brdflut", std::move(brdfImage));

	auto shader_id = EGGraphics->addShader("brdflut_generator", "brdflut_generator");
	auto& pShader = EGGraphics->getShader(shader_id);

	pShader->addTexture("outColour", output_id);

	pShader->dispatch(size);

	EGGraphics->removeShader(shader_id);

	return output_id;
}

size_t CEnvironmentSystem::computeIrradiance(size_t origin, uint32_t size)
{
	auto pDevice = EGGraphics->getDevice().get();
	auto irradianceCubemap = std::make_unique<CImageCubemap>(pDevice);
	irradianceCubemap->create(
		vk::Extent2D{ size, size }, 
		vk::Format::eR32G32B32A32Sfloat, 
		vk::ImageLayout::eGeneral,
		vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | 
		vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | 
		vk::ImageUsageFlagBits::eStorage);

	auto output_id = EGGraphics->addImage("irradiance", std::move(irradianceCubemap));

	auto shader_id = EGGraphics->addShader("irradiancecube_generator", "irradiancecube_generator");
	auto& pShader = EGGraphics->getShader(shader_id);
	pShader->addTexture("outColour", output_id);
	pShader->addTexture("samplerColour", origin);

	pShader->dispatch(size);

	EGGraphics->removeShader(shader_id);

	return output_id;
}

size_t CEnvironmentSystem::computePrefiltered(size_t origin, uint32_t size)
{
	auto pDevice = EGGraphics->getDevice().get();
	auto prefilteredCubemap = std::make_unique<CImageCubemap>(pDevice);
	prefilteredCubemap->create(
		vk::Extent2D{ size, size }, 
		vk::Format::eR16G16B16A16Sfloat, 
		vk::ImageLayout::eGeneral,
		vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | 
		vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | 
		vk::ImageUsageFlagBits::eStorage,
		vk::ImageAspectFlagBits::eColor, 
		vk::Filter::eLinear, 
		vk::SamplerAddressMode::eClampToEdge, 
		vk::SampleCountFlagBits::e1, true, false, true);

	auto output_id = EGGraphics->addImage("prefiltered", std::move(prefilteredCubemap));
	auto& target = EGGraphics->getImage(output_id);

	auto shader_id = EGGraphics->addShader("prefilteredmap_generator", "prefilteredmap_generator");
	auto& pShader = EGGraphics->getShader(shader_id);
	auto& pPushConst = pShader->getPushBlock("object");

	auto cmdBuf = CCommandBuffer(pDevice);
	cmdBuf.create(false, vk::QueueFlagBits::eCompute);

	for (uint32_t i = 0; i < target->getMipLevels(); i++)
	{
		vk::ImageView levelView = VK_NULL_HANDLE;
		vk::ImageViewCreateInfo viewInfo{};
		viewInfo.viewType = vk::ImageViewType::eCube;
		viewInfo.format = target->getFormat();
		viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		viewInfo.subresourceRange.baseMipLevel = i;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 6;
		viewInfo.image = target->getImage();

		vk::Result res = pDevice->create(viewInfo, &levelView);
		assert(res == vk::Result::eSuccess && "Cannot create image view.");

		cmdBuf.begin();
		auto commandBuffer = cmdBuf.getCommandBuffer();

		auto imageInfo = target->getDescriptor();
		imageInfo.imageView = levelView;

		pPushConst->set("roughness", static_cast<float>(i) / static_cast<float>(target->getMipLevels() - 1));
		pPushConst->flush(commandBuffer);

		pShader->addTexture("outColour", imageInfo);
		pShader->addTexture("samplerColour", origin);

		pShader->dispatch(commandBuffer, size);
		cmdBuf.submitIdle();

		pDevice->destroy(&levelView);
	}

	EGGraphics->removeShader(shader_id);

	return output_id;
}