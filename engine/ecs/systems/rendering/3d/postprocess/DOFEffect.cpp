#include "DOFEffect.h"

#include "Engine.h"
#include "EffectShared.h"

#include "ecs/components/CameraComponent.h"
#include "ecs/helper.hpp"

using namespace engine::graphics;
using namespace engine::ecs;

void CDOFEffect::create()
{
	init();

	shader_dof = EGGraphics->addShader("dof", "dof");

	FShaderSpecials specials;
	specials.usages = 4;
	shader_blur = EGGraphics->addShader("gaussian_blur", "gaussian_blur", specials);
}

void CDOFEffect::update()
{
	auto& device = EGGraphics->getDevice();
	auto& image = EGGraphics->getImage(blur_image);
	auto extent = device->getExtent(true);
	auto img_extent = image->getExtent();

	if (extent.width != img_extent.width || extent.height != img_extent.height)
		init();
}

void CDOFEffect::init()
{
	EGGraphics->removeImage(blur_image);
	EGGraphics->removeImage(temp_image);
	blur_image = effectshared::createImage("dof_blur_tex", vk::Format::eB10G11R11UfloatPack32);
	temp_image = effectshared::createImage("dof_temp_tex", vk::Format::eB10G11R11UfloatPack32);
}

size_t CDOFEffect::render(bool enable, size_t depth_source, size_t in_source, size_t out_source)
{
	auto& device = EGGraphics->getDevice();
	auto extent = device->getExtent(true);
	auto resolution = glm::vec2(static_cast<float>(extent.width), static_cast<float>(extent.height));
	auto& peffects = EGEngine->getPostEffects();
	auto commandBuffer = EGGraphics->getCommandBuffer();

	if (enable)
	{
		// Blur in image
		//{
		//	auto& pShader = EGGraphics->getShader(shader_blur);
		//
		//	auto& pPush = pShader->getPushBlock("ubo");
		//	pPush->set("size", peffects.dof_bokeh_size);
		//	pPush->set("separation", peffects.dof_bokeh_separation);
		//	pPush->set("minThreshold", peffects.dof_bokeh_min_threshold);
		//	pPush->set("maxThreshold", peffects.dof_bokeh_max_threshold);
		//	pPush->flush(commandBuffer);
		//
		//	pShader->addTexture("writeColor", blur_image);
		//	pShader->addTexture("samplerColor", in_source);
		//
		//	pShader->dispatch(commandBuffer, resolution);
		//}

		{
			auto& pShader = EGGraphics->getShader(shader_blur);

			auto& pPush = pShader->getPushBlock("ubo");
			pPush->set("blur_scale", peffects.dof_blur_scale);
			pPush->set("blur_strength", peffects.dof_blur_strength);
			pPush->set("direction", -1);
			pPush->flush(commandBuffer);

			pShader->addTexture("writeColor", temp_image);
			pShader->addTexture("samplerColor", in_source);

			pShader->dispatch(commandBuffer, resolution);

			VkHelper::BarrierFromComputeToCompute(commandBuffer);

			pPush->set("blur_scale", peffects.dof_blur_scale);
			pPush->set("blur_strength", peffects.dof_blur_strength);
			pPush->set("direction", 1);
			pPush->flush(commandBuffer);

			pShader->addTexture("writeColor", blur_image);
			pShader->addTexture("samplerColor", temp_image);

			pShader->dispatch(commandBuffer, resolution);

			VkHelper::BarrierFromComputeToCompute(commandBuffer);

			pPush->set("blur_scale", peffects.dof_blur_scale);
			pPush->set("blur_strength", peffects.dof_blur_strength);
			pPush->set("direction", -1);
			pPush->flush(commandBuffer);

			pShader->addTexture("writeColor", temp_image);
			pShader->addTexture("samplerColor", blur_image);

			pShader->dispatch(commandBuffer, resolution);

			VkHelper::BarrierFromComputeToCompute(commandBuffer);

			pPush->set("blur_scale", peffects.dof_blur_scale);
			pPush->set("blur_strength", peffects.dof_blur_strength);
			pPush->set("direction", 1);
			pPush->flush(commandBuffer);

			pShader->addTexture("writeColor", blur_image);
			pShader->addTexture("samplerColor", temp_image);

			pShader->dispatch(commandBuffer, resolution);
		}	

		VkHelper::BarrierFromComputeToCompute(commandBuffer);

		// Apply depth of field
		{
			auto& registry = EGCoordinator;
			auto editorMode = EGEngine->isEditorMode();
			auto state = EGEngine->getState();

			FCameraComponent* camera{ nullptr };

			if (editorMode && state == EEngineState::eEditing)
				camera = registry.try_get<FCameraComponent>(EGEditor->getCamera());
			else
				camera = registry.try_get<FCameraComponent>(get_active_camera(registry));

			auto& pShader = EGGraphics->getShader(shader_dof);
			auto& pPush = pShader->getPushBlock("ubo");

			pPush->set("nearPlane", camera->nearPlane);
			pPush->set("farPlane", camera->farPlane);

			pPush->set("focusPoint", peffects.dof_focus_point);
			pPush->set("nearField", peffects.dof_near_field);
			pPush->set("nearTransition", peffects.dof_near_transition);
			pPush->set("farField", peffects.dof_far_field);
			pPush->set("farTransition", peffects.dof_far_transition);
			pPush->flush(commandBuffer);

			pShader->addTexture("writeColor", temp_image);
			pShader->addTexture("samplerDepth", depth_source);
			pShader->addTexture("samplerColor", in_source);
			pShader->addTexture("samplerBlured", blur_image);

			pShader->dispatch(commandBuffer, resolution);

			VkHelper::BarrierFromComputeToCompute(commandBuffer);
		}

		return temp_image;
	}

	return in_source;
}