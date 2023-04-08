#include "FilmGrainEffect.h"

#include "Engine.h"

#include "ecs/components/CameraComponent.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CFilmGrainEffect::create()
{
	graphics = EGEngine->getGraphics().get();

	shader_grain = graphics->addShader("filmgrain", "filmgrain");
}

size_t CFilmGrainEffect::render(FCameraComponent* camera, float time, size_t in_source, size_t out_source)
{
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);
	
	FDispatchParam param;
	param.size = { extent.width, extent.height, 1.f };

	if (camera->effects.filmgrain.enable)
	{
		graphics->bindShader(shader_grain);

		graphics->bindTexture("writeColor", out_source);
		graphics->bindTexture("samplerColor", in_source);

		auto& pPush = graphics->getPushBlockHandle("ubo");
		pPush->set("amount", camera->effects.filmgrain.amount);
		pPush->set("time", time);

		graphics->dispatch(param);

		graphics->bindShader(invalid_index);

		graphics->BarrierFromComputeToCompute();

		return out_source;
	}

	return in_source;
}