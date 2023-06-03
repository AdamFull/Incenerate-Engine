#include "FXAAEffect.h"

#include "Engine.h"

#include "ecs/components/CameraComponent.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CFXAAEffect::create()
{
	graphics = EGEngine->getGraphics().get();
	shader_fxaa = graphics->addShader("fxaa");
}

size_t CFXAAEffect::render(FCameraComponent* camera, size_t in_source, size_t out_source)
{
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);
	
	FDispatchParam param;
	param.size = { extent.width, extent.height, 1.f };

	if (camera->effects.fxaa.enable)
	{
		graphics->bindShader(shader_fxaa);

		graphics->bindTexture("writeColor", out_source);
		graphics->bindTexture("samplerColor", in_source);

		auto& pPush = graphics->getPushBlockHandle("ubo");
		pPush->set("quality", atof(fxaa_quality_variant[camera->effects.fxaa.quality]));
		pPush->set("threshold", atof(fxaa_threshold_variant[camera->effects.fxaa.threshold]));
		pPush->set("threshold_min", atof(fxaa_threshold_min_variant[camera->effects.fxaa.threshold_min]));

		graphics->dispatch(param);

		graphics->bindShader(invalid_index);

		graphics->BarrierFromComputeToCompute();

		return out_source;
	}

	return in_source;
}