#include "VignetteEffect.h"

#include "Engine.h"

#include "ecs/components/CameraComponent.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CVignetteEffect::create()
{
	shader_vignette = EGGraphics->addShader("chromatic_aberration", "chromatic_aberration");
}

size_t CVignetteEffect::render(FCameraComponent* camera, size_t in_source, size_t out_source)
{
	auto& device = EGGraphics->getDevice();
	auto extent = device->getExtent(true);
	auto resolution = glm::vec2(static_cast<float>(extent.width), static_cast<float>(extent.height));
	auto commandBuffer = EGGraphics->getCommandBuffer();

	if (camera->effects.vignette.enable)
	{
		auto& pShader = EGGraphics->getShader(shader_vignette);

		pShader->addTexture("writeColor", out_source);
		pShader->addTexture("samplerColor", in_source);

		auto& pPush = pShader->getPushBlock("ubo");
		pPush->set("inner_radius", camera->effects.vignette.inner);
		pPush->set("outer_radius", camera->effects.vignette.outer);
		pPush->set("opacity", camera->effects.vignette.opacity);
		pPush->flush(commandBuffer);

		pShader->dispatch(commandBuffer, resolution);

		VkHelper::BarrierFromComputeToCompute(commandBuffer);

		return out_source;
	}

	return in_source;
}