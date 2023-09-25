#include "ChromaticAberrationEffect.h"

#include "Engine.h"

#include "ecs/components/CameraComponent.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CChromaticAberrationEffect::create()
{
	graphics = EGEngine->getGraphics().get();

	FShaderCreateInfo specials;
	specials.bind_point = vk::PipelineBindPoint::eCompute;
	specials.usages = 1;
	shader_aberr = graphics->addShader("postprocess:chromatic_aberration", specials);
}

size_t CChromaticAberrationEffect::render(FCameraComponent* camera, size_t in_source, size_t out_source)
{
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);
	
	FDispatchParam param;
	param.size = { extent.width, extent.height, 1.f };

	if (camera->effects.aberration.enable)
	{
		graphics->bindShader(shader_aberr);

		graphics->bindTexture("writeColor", out_source);
		graphics->bindTexture("samplerColor", in_source);

		auto& pPush = graphics->getPushBlockHandle("ubo");
		pPush->set("distortion", camera->effects.aberration.distortion);
		pPush->set("iterations", camera->effects.aberration.iterations);

		graphics->dispatch(param);

		graphics->bindShader(invalid_index);

		graphics->BarrierFromComputeToCompute();

		return out_source;
	}

	return in_source;
}