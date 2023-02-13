#include "PostProcessSystem.h"
#include "Engine.h"

#include "postprocess/EffectShared.h"

#include "ecs/components/CameraComponent.h"
#include "ecs/helper.hpp"

using namespace engine::graphics;
using namespace engine::ecs;
using namespace engine::system::window;

CPostProcessSystem::~CPostProcessSystem()
{
	
}

void CPostProcessSystem::__create()
{
	fxaa.create();
	dof.create();
	bloom.create();
	chromatic_aberration.create();
	vignette.create();
	filmgrain.create();
	fog.create();
	tonemap.create();

	final_image = effectshared::createImage("postprocess_tex", vk::Format::eR32G32B32A32Sfloat);
	auto& image = graphics->getImage(final_image);

	shader_id = graphics->addShader("emptypass", "emptypass");

	addSubresource("composition_tex");
	addSubresource("depth_tex");

	CBaseGraphicsSystem::__create();
}

void CPostProcessSystem::__update(float fDt)
{
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);

	FDispatchParam param;
	param.size = { extent.width, extent.height, 1.f };

	auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	{
		effectshared::tryReCreateImage("postprocess_tex", final_image, vk::Format::eR32G32B32A32Sfloat);
		dof.update();
		bloom.update();
	}

	VkHelper::BarrierFromGraphicsToCompute(getSubresource("composition_tex"));
	VkHelper::BarrierFromGraphicsToCompute(getSubresource("depth_tex"));

	size_t current_image = getSubresource("composition_tex");
	current_image = tonemap.render(camera, fDt, current_image, final_image);
	current_image = fxaa.render(camera, current_image, final_image);
	current_image = dof.render(camera, getSubresource("depth_tex"), current_image, final_image);
	current_image = chromatic_aberration.render(camera, current_image, final_image);
	current_image = vignette.render(camera, current_image, final_image);
	current_image = filmgrain.render(camera, fDt, current_image, final_image);
	current_image = fog.render(camera, getSubresource("depth_tex"), current_image, final_image);
	current_image = bloom.render(camera, current_image);
	
	if (current_image != final_image)
	{
		graphics->bindShader(shader_id);

		graphics->bindTexture("writeColor", final_image);
		graphics->bindTexture("samplerColor", current_image);

		graphics->dispatch(param);

		graphics->bindShader(invalid_index);
	}

	VkHelper::BarrierFromComputeToGraphics();
}