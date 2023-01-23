#include "TonemapEffect.h"

#include "Engine.h"

#include "ecs/components/CameraComponent.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CTonemapEffect::create()
{
	shader_tonemap = EGGraphics->addShader("tonemap", "tonemap");
}

size_t CTonemapEffect::render(FCameraComponent* camera, size_t in_source, size_t out_source)
{
	auto& device = EGGraphics->getDevice();
	auto extent = device->getExtent(true);
	auto resolution = glm::vec2(static_cast<float>(extent.width), static_cast<float>(extent.height));
	auto commandBuffer = EGGraphics->getCommandBuffer();

	if (camera->effects.tonemap.enable)
	{
		auto& pShader = EGGraphics->getShader(shader_tonemap);

		auto& pBlock = pShader->getPushBlock("ubo");
		pBlock->set("gamma", camera->effects.tonemap.gamma);
		pBlock->set("exposure", camera->effects.tonemap.exposure);
		pBlock->flush(commandBuffer);

		pShader->addTexture("writeColor", out_source);
		pShader->addTexture("samplerColor", in_source);

		pShader->dispatch(commandBuffer, resolution);

		return out_source;
	}

	return in_source;
}