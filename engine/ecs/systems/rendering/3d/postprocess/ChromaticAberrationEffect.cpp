#include "ChromaticAberrationEffect.h"

#include "Engine.h"

#include "ecs/components/CameraComponent.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CChromaticAberrationEffect::create()
{
	shader_aberr = EGGraphics->addShader("chromatic_aberration", "chromatic_aberration");
}

size_t CChromaticAberrationEffect::render(FCameraComponent* camera, size_t in_source, size_t out_source)
{
	auto& device = EGGraphics->getDevice();
	auto extent = device->getExtent(true);
	auto resolution = glm::vec2(static_cast<float>(extent.width), static_cast<float>(extent.height));
	auto commandBuffer = EGGraphics->getCommandBuffer();

	if (camera->effects.aberration.enable)
	{
		auto& pShader = EGGraphics->getShader(shader_aberr);

		pShader->addTexture("writeColor", out_source);
		pShader->addTexture("samplerColor", in_source);

		auto& pPush = pShader->getPushBlock("ubo");
		pPush->set("distortion", camera->effects.aberration.distortion);
		pPush->set("iterations", camera->effects.aberration.iterations);
		pPush->flush(commandBuffer);

		pShader->dispatch(commandBuffer, resolution);

		VkHelper::BarrierFromComputeToCompute(commandBuffer);

		return out_source;
	}

	return in_source;
}