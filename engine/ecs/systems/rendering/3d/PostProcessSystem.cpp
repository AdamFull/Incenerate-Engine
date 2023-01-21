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
	bloom.create();

	final_image = effectshared::createImage("postprocess_tex", vk::Format::eB10G11R11UfloatPack32);
	auto& image = EGGraphics->getImage(final_image);

	
	shader_tonemap = EGGraphics->addShader("tonemap", "tonemap");

	addSubresource("composition_tex");

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
		effectshared::tryReCreateImage("postprocess_tex", final_image, vk::Format::eB10G11R11UfloatPack32);
		bloom.update();
	}

	VkHelper::BarrierFromGraphicsToCompute(commandBuffer, getSubresource("composition_tex"));

	size_t current_image = getSubresource("composition_tex");
	current_image = fxaa.render(peffects.fxaa, current_image, final_image);
	current_image = bloom.render(peffects.bloom, current_image);
	
	// Tonemap
	{
		auto& pShader = EGGraphics->getShader(shader_tonemap);
	
		auto& pBlock = pShader->getPushBlock("ubo");
		pBlock->set("gamma", peffects.gamma);
		pBlock->set("exposure", peffects.exposure);
		pBlock->flush(commandBuffer);
	
		pShader->addTexture("writeColor", final_image);
		pShader->addTexture("samplerColor", current_image);
	
		pShader->dispatch(commandBuffer, resolution);
	}

	VkHelper::BarrierFromComputeToGraphics(commandBuffer);
}