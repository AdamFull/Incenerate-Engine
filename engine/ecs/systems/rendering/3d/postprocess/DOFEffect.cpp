#include "DOFEffect.h"

#include "Engine.h"
#include "EffectShared.h"

#include "ecs/components/CameraComponent.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CDOFEffect::create()
{
	graphics = EGEngine->getGraphics().get();
	init();

	shader_dof = graphics->addShader("dof");

	FShaderSpecials specials;
	specials.usages = 1;
	shader_blur = graphics->addShader("bokeh_blur", specials);
}

void CDOFEffect::update()
{
	auto& device = graphics->getDevice();
	auto& image = graphics->getImage(blur_image);
	auto extent = device->getExtent(true);
	auto img_extent = image->getExtent();

	if (extent.width != img_extent.width || extent.height != img_extent.height)
		init();
}

void CDOFEffect::init()
{
	graphics->removeImage(blur_image);
	graphics->removeImage(temp_image);
	blur_image = effectshared::createImage("dof_blur_tex", vk::Format::eB10G11R11UfloatPack32);
	temp_image = effectshared::createImage("dof_temp_tex", vk::Format::eB10G11R11UfloatPack32);
}

size_t CDOFEffect::render(FCameraComponent* camera, size_t depth_source, size_t in_source, size_t out_source)
{
	auto& device = graphics->getDevice();
	auto extent = device->getExtent(true);
	
	FDispatchParam param;
	param.size = { extent.width, extent.height, 1.f };

	if (camera->effects.dof.enable)
	{
		// Blur in image
		{
			graphics->bindShader(shader_blur);
		
			auto& pPush = graphics->getPushBlockHandle("ubo");
			pPush->set("max_radius", camera->effects.dof.bokeh_samples);
			pPush->set("sides", camera->effects.dof.bokeh_poly);
		
			graphics->bindTexture("writeColor", blur_image);
			graphics->bindTexture("samplerColor", in_source);
		
			graphics->dispatch(param);

			graphics->bindShader(invalid_index);
		}

		graphics->BarrierFromComputeToCompute();

		// Apply depth of field
		{
			graphics->bindShader(shader_dof);
			auto& pPush = graphics->getPushBlockHandle("ubo");

			pPush->set("nearPlane", camera->nearPlane);
			pPush->set("farPlane", camera->farPlane);

			pPush->set("focusPoint", camera->effects.dof.focus_point);
			pPush->set("nearField", camera->effects.dof.near_field);
			pPush->set("nearTransition", camera->effects.dof.near_transition);
			pPush->set("farField", camera->effects.dof.far_field);
			pPush->set("farTransition", camera->effects.dof.far_transition);

			graphics->bindTexture("writeColor", temp_image);
			graphics->bindTexture("samplerDepth", depth_source);
			graphics->bindTexture("samplerColor", in_source);
			graphics->bindTexture("samplerBlured", blur_image);

			graphics->dispatch(param);

			graphics->bindShader(invalid_index);

			graphics->BarrierFromComputeToCompute();
		}

		return temp_image;
	}

	return in_source;
}