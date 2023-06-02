#include "TonemapEffect.h"

#include "Engine.h"
#include "EffectShared.h"

#include "ecs/components/CameraComponent.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CTonemapEffect::create()
{
	graphics = EGEngine->getGraphics().get();
	shader_tonemap = graphics->addShader("tonemap");
	brightness_image = effectshared::createImage("brightness_image", vk::Format::eR32Sfloat);
}

size_t CTonemapEffect::render(FCameraComponent* camera, float time, size_t in_source, size_t out_source)
{
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);

	std::vector<FDispatchParam> params;
	params.resize(3);

	// Histogram
	params[0].size = { extent.width, extent.height, 1.f };
	// Average
	params[1].size = { 1.f, 1.f, 1.f };
	params[1].divideByLocalSizes = false;
	// Tonemap
	params[2].size = params[0].size;

	if (camera->effects.tonemap.enable)
	{
		graphics->bindShader(shader_tonemap);

		auto& pBlock = graphics->getPushBlockHandle("ubo");
		pBlock->set("fxaa", static_cast<int>(camera->effects.fxaa.enable));
		pBlock->set("gamma", camera->effects.tonemap.gamma);
		pBlock->set("exposure", camera->effects.tonemap.exposure);
		pBlock->set("whitePoint", camera->effects.tonemap.whitePoint);
		pBlock->set("adaptive", static_cast<int32_t>(camera->effects.tonemap.adaptive));
		graphics->flushConstantRanges(pBlock);

		float logLumRange = camera->effects.tonemap.lumMax - camera->effects.tonemap.lumMin;
		float timeCoeff = glm::clamp<float>(1.0f - glm::exp(-time * camera->effects.tonemap.tau), 0.0, 1.0);

		auto& pParams = graphics->getUniformHandle("UBOParams");
		pParams->set("minLogLum", camera->effects.tonemap.lumMin);
		pParams->set("logLumRange", logLumRange);
		pParams->set("timeCoeff", timeCoeff);
		pParams->set("numPixelsX", static_cast<float>(extent.width));
		pParams->set("numPixelsY", static_cast<float>(extent.height));

		graphics->bindTexture("writeColor", out_source);
		graphics->bindTexture("samplerColor", in_source);
		graphics->bindTexture("target", brightness_image);

		graphics->dispatch(params);

		graphics->bindShader(invalid_index);

		graphics->BarrierFromComputeToCompute();

		return out_source;
	}

	return in_source;
}