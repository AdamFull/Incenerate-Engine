#include "TonemapEffect.h"

#include "Engine.h"
#include "EffectShared.h"

#include "ecs/components/CameraComponent.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CTonemapEffect::create()
{
	graphics = EGEngine->getGraphics().get();
	shader_tonemap = graphics->addShader("tonemap", "tonemap");
	brightness_image = effectshared::createImage("brightness_image", vk::Format::eR32Sfloat);
}

size_t CTonemapEffect::render(FCameraComponent* camera, float time, size_t in_source, size_t out_source)
{
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);
	std::vector<glm::vec3> sizes{ glm::vec3(0.f), glm::vec3(0.f), glm::vec3(extent.width, extent.height, 1u) };

	if (camera->effects.tonemap.enable)
	{
		graphics->bindShader(shader_tonemap);

		auto& pBlock = graphics->getPushBlockHandle("ubo");
		pBlock->set("fxaa", static_cast<int>(camera->effects.fxaa.enable));
		pBlock->set("gamma", camera->effects.tonemap.gamma);
		pBlock->set("exposure", camera->effects.tonemap.exposure);
		pBlock->set("whitePoint", camera->effects.tonemap.whitePoint);
		pBlock->set("adaptive", static_cast<int32_t>(camera->effects.tonemap.adaptive));

		auto& pParams = graphics->getUniformHandle("UBOParams");
		pParams->set("u_params", glm::vec4(camera->effects.tonemap.lumMin, camera->effects.tonemap.lumRange, time, sizes[2].x * sizes[2].y));

		graphics->bindTexture("writeColor", out_source);
		graphics->bindTexture("samplerColor", in_source);
		graphics->bindTexture("target", brightness_image);

		graphics->dispatch(sizes);

		graphics->bindShader(invalid_index);

		VkHelper::BarrierFromComputeToCompute();

		return out_source;
	}

	return in_source;
}