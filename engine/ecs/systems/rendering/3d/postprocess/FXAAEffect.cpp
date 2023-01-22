#include "FXAAEffect.h"

#include "Engine.h"

#include "ecs/components/CameraComponent.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CFXAAEffect::create()
{
	shader_fxaa = EGGraphics->addShader("fxaa", "fxaa");
}

size_t CFXAAEffect::render(FCameraComponent* camera, size_t in_source, size_t out_source)
{
	auto& device = EGGraphics->getDevice();
	auto extent = device->getExtent(true);
	auto resolution = glm::vec2(static_cast<float>(extent.width), static_cast<float>(extent.height));
	auto commandBuffer = EGGraphics->getCommandBuffer();

	if (camera->effects.fxaa.enable)
	{
		auto& pShader = EGGraphics->getShader(shader_fxaa);

		pShader->addTexture("writeColor", out_source);
		pShader->addTexture("samplerColor", in_source);

		auto& pPush = pShader->getPushBlock("ubo");
		pPush->set("texel_step", glm::vec2(1.f / extent.width, 1.f / extent.height));
		pPush->set("quality", camera->effects.fxaa.quality);
		pPush->flush(commandBuffer);

		pShader->dispatch(commandBuffer, resolution);

		VkHelper::BarrierFromComputeToCompute(commandBuffer);

		return out_source;
	}

	return in_source;
}