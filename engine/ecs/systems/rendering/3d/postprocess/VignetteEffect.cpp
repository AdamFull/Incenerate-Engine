#include "VignetteEffect.h"

#include "Engine.h"

#include "ecs/components/CameraComponent.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CVignetteEffect::create()
{
	graphics = EGEngine->getGraphics().get();
	shader_vignette = graphics->addShader("vignette", "vignette");
}

size_t CVignetteEffect::render(FCameraComponent* camera, size_t in_source, size_t out_source)
{
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);
	
	FDispatchParam param;
	param.size = { extent.width, extent.height, 1.f };

	if (camera->effects.vignette.enable)
	{
		graphics->bindShader(shader_vignette);

		graphics->bindTexture("writeColor", out_source);
		graphics->bindTexture("samplerColor", in_source);

		auto& pPush = graphics->getPushBlockHandle("ubo");
		pPush->set("inner_radius", camera->effects.vignette.inner);
		pPush->set("outer_radius", camera->effects.vignette.outer);
		pPush->set("opacity", camera->effects.vignette.opacity);

		graphics->dispatch(param);

		graphics->bindShader(invalid_index);

		VkHelper::BarrierFromComputeToCompute();

		return out_source;
	}

	return in_source;
}