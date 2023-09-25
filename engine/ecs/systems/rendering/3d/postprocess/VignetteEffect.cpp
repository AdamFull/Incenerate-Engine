#include "VignetteEffect.h"

#include "Engine.h"

#include "ecs/components/CameraComponent.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CVignetteEffect::create()
{
	graphics = EGEngine->getGraphics().get();

	FShaderCreateInfo specials;
	specials.bind_point = vk::PipelineBindPoint::eCompute;
	specials.usages = 1;

	shader_vignette = graphics->addShader("postprocess:vignette", specials);
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

		graphics->BarrierFromComputeToCompute();

		return out_source;
	}

	return in_source;
}