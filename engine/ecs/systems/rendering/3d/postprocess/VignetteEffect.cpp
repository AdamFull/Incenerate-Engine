#include "VignetteEffect.h"

#include "Engine.h"

#include "ecs/components/CameraComponent.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CVignetteEffect::create()
{
	shader_vignette = EGGraphics->addShader("vignette", "vignette");
}

size_t CVignetteEffect::render(FCameraComponent* camera, size_t in_source, size_t out_source)
{
	auto& graphics = EGGraphics;
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);
	auto resolution = glm::vec2(static_cast<float>(extent.width), static_cast<float>(extent.height));

	if (camera->effects.vignette.enable)
	{
		graphics->bindShader(shader_vignette);

		graphics->bindTexture("writeColor", out_source);
		graphics->bindTexture("samplerColor", in_source);

		auto& pPush = graphics->getPushBlockHandle("ubo");
		pPush->set("inner_radius", camera->effects.vignette.inner);
		pPush->set("outer_radius", camera->effects.vignette.outer);
		pPush->set("opacity", camera->effects.vignette.opacity);

		graphics->dispatch(resolution);

		graphics->bindShader(invalid_index);

		VkHelper::BarrierFromComputeToCompute();

		return out_source;
	}

	return in_source;
}