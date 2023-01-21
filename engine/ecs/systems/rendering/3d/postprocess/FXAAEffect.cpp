#include "FXAAEffect.h"

#include "Engine.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CFXAAEffect::create()
{
	shader_fxaa = EGGraphics->addShader("fxaa", "fxaa");
}

size_t CFXAAEffect::render(bool enable, size_t in_source, size_t out_source)
{
	auto& device = EGGraphics->getDevice();
	auto extent = device->getExtent(true);
	auto resolution = glm::vec2(static_cast<float>(extent.width), static_cast<float>(extent.height));
	auto& peffects = EGEngine->getPostEffects();
	auto commandBuffer = EGGraphics->getCommandBuffer();

	if (enable)
	{
		auto& pShader = EGGraphics->getShader(shader_fxaa);

		pShader->addTexture("writeColor", out_source);
		pShader->addTexture("samplerColor", in_source);

		auto& pPush = pShader->getPushBlock("ubo");
		pPush->set("texel_step", glm::vec2(1.f / extent.width, 1.f / extent.height));
		pPush->set("quality", peffects.fxaa_quality);
		pPush->flush(commandBuffer);

		pShader->dispatch(commandBuffer, resolution);

		VkHelper::BarrierFromComputeToCompute(commandBuffer);

		return out_source;
	}

	return in_source;
}