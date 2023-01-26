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
	auto& graphics = EGGraphics;
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);
	auto resolution = glm::vec2(static_cast<float>(extent.width), static_cast<float>(extent.height));

	if (camera->effects.fxaa.enable)
	{
		graphics->bindShader(shader_fxaa);

		graphics->bindTexture("writeColor", out_source);
		graphics->bindTexture("samplerColor", in_source);

		auto& pPush = graphics->getPushBlockHandle("ubo");
		pPush->set("texel_step", glm::vec2(1.f / extent.width, 1.f / extent.height));
		pPush->set("quality", camera->effects.fxaa.quality);

		graphics->dispatch(resolution);

		graphics->bindShader(invalid_index);

		VkHelper::BarrierFromComputeToCompute();

		return out_source;
	}

	return in_source;
}