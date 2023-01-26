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
	auto& graphics = EGGraphics;
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);
	auto resolution = glm::vec2(static_cast<float>(extent.width), static_cast<float>(extent.height));

	if (camera->effects.tonemap.enable)
	{
		graphics->bindShader(shader_tonemap);

		auto& pBlock = graphics->getPushBlockHandle("ubo");
		pBlock->set("gamma", camera->effects.tonemap.gamma);
		pBlock->set("exposure", camera->effects.tonemap.exposure);

		graphics->bindTexture("writeColor", out_source);
		graphics->bindTexture("samplerColor", in_source);

		graphics->dispatch(resolution);

		graphics->bindShader(invalid_index);

		return out_source;
	}

	return in_source;
}