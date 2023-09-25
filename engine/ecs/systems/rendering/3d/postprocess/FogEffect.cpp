#include "FogEffect.h"

#include "Engine.h"

#include "ecs/components/CameraComponent.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CFogEffect::create()
{
	graphics = EGEngine->getGraphics().get();

	FShaderCreateInfo specials;
	specials.bind_point = vk::PipelineBindPoint::eCompute;
	specials.usages = 1;

	shader_fog = graphics->addShader("postprocess:fog", specials);
}

size_t CFogEffect::render(FCameraComponent* camera, size_t depth_source, size_t in_source, size_t out_source)
{
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);
	
	FDispatchParam param;
	param.size = { extent.width, extent.height, 1.f };

	if (camera->effects.fog.enable)
	{
		graphics->bindShader(shader_fog);

		graphics->bindTexture("writeColor", out_source);
		graphics->bindTexture("samplerColor", in_source);
		graphics->bindTexture("samplerDepth", depth_source);

		auto& pPush = graphics->getPushBlockHandle("ubo");
		pPush->set("color", camera->effects.fog.color);
		pPush->set("cameraPos", camera->viewPos);
		pPush->set("invViewProjection", glm::inverse(camera->projection * camera->view));
		pPush->set("fogDensity", camera->effects.fog.density);
		pPush->set("fogMax", camera->farPlane);

		graphics->dispatch(param);

		graphics->bindShader(invalid_index);

		graphics->BarrierFromComputeToCompute();

		return out_source;
	}

	return in_source;
}