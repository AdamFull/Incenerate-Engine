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

	final_image = effectshared::createImage("postprocess_tex", vk::Format::eR32G32B32A32Sfloat);
	auto& image = EGGraphics->getImage(final_image);

	
	shader_tonemap = EGGraphics->addShader("tonemap", "tonemap");

	addSubresource("composition_tex");
	addSubresource("depth_tex");

	CBaseGraphicsSystem::__create();
}

void CPostProcessSystem::__update(float fDt)
{
	auto& device = EGGraphics->getDevice();
	auto extent = device->getExtent(true);
	auto resolution = glm::vec2(static_cast<float>(extent.width), static_cast<float>(extent.height));
	auto commandBuffer = EGGraphics->getCommandBuffer();

	auto& registry = EGCoordinator;
	auto editorMode = EGEngine->isEditorMode();
	auto state = EGEngine->getState();

	FCameraComponent* camera{ nullptr };

	if (editorMode && state == EEngineState::eEditing)
		camera = registry.try_get<FCameraComponent>(EGEditor->getCamera());
	else
		camera = registry.try_get<FCameraComponent>(get_active_camera(registry));

	{
		effectshared::tryReCreateImage("postprocess_tex", final_image, vk::Format::eR32G32B32A32Sfloat);
		dof.update();
		bloom.update();
	}

	VkHelper::BarrierFromGraphicsToCompute(commandBuffer, getSubresource("composition_tex"));
	VkHelper::BarrierFromGraphicsToCompute(commandBuffer, getSubresource("depth_tex"));

	//EGGraphics->copyImage(commandBuffer, getSubresource("composition_tex"), temp_image);

	size_t current_image = getSubresource("composition_tex");
	current_image = fxaa.render(camera, current_image, final_image);
	current_image = bloom.render(camera, current_image);
	current_image = dof.render(camera, getSubresource("depth_tex"), current_image, final_image);
	current_image = chromatic_aberration.render(camera, current_image, final_image);
	current_image = vignette.render(camera, current_image, final_image);
	
	// Tonemap
	{
		auto& pShader = EGGraphics->getShader(shader_tonemap);
	
		auto& pBlock = pShader->getPushBlock("ubo");
		pBlock->set("gamma", camera->effects.tonemap.gamma);
		pBlock->set("exposure", camera->effects.tonemap.exposure);
		pBlock->flush(commandBuffer);
	
		pShader->addTexture("writeColor", final_image);
		pShader->addTexture("samplerColor", current_image);
	
		pShader->dispatch(commandBuffer, resolution);
	}

	VkHelper::BarrierFromComputeToGraphics(commandBuffer);
}