#include "PostProcessSystem.h"
#include "Engine.h"

#include "postprocess/EffectShared.h"

#ifdef MemoryBarrier
#undef MemoryBarrier
#endif

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
	auto& peffects = EGEngine->getPostEffects();
	auto commandBuffer = EGGraphics->getCommandBuffer();

	{
		effectshared::tryReCreateImage("postprocess_tex", final_image, vk::Format::eR32G32B32A32Sfloat);
		dof.update();
		bloom.update();
	}

	VkHelper::BarrierFromGraphicsToCompute(commandBuffer, getSubresource("composition_tex"));
	VkHelper::BarrierFromGraphicsToCompute(commandBuffer, getSubresource("depth_tex"));

	//EGGraphics->copyImage(commandBuffer, getSubresource("composition_tex"), temp_image);

	size_t current_image = getSubresource("composition_tex");
	current_image = fxaa.render(peffects.fxaa, current_image, final_image);
	current_image = bloom.render(peffects.bloom, current_image);
	current_image = dof.render(peffects.dof, getSubresource("depth_tex"), current_image, final_image);
	current_image = chromatic_aberration.render(peffects.aberration, current_image, final_image);
	current_image = vignette.render(peffects.vignette, current_image, final_image);
	
	// Tonemap
	{
		auto& pShader = EGGraphics->getShader(shader_tonemap);
	
		auto& pBlock = pShader->getPushBlock("ubo");
		pBlock->set("gamma", peffects.tonemapping_gamma);
		pBlock->set("exposure", peffects.tonemapping_exposure);
		pBlock->flush(commandBuffer);
	
		pShader->addTexture("writeColor", final_image);
		pShader->addTexture("samplerColor", current_image);
	
		pShader->dispatch(commandBuffer, resolution);
	}

	VkHelper::BarrierFromComputeToGraphics(commandBuffer);
}