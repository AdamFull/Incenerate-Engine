#include "PostProcessSystem.h"
#include "Engine.h"

using namespace engine::ecs;
using namespace engine::system::window;

CPostProcessSystem::~CPostProcessSystem()
{
	
}

void CPostProcessSystem::__create()
{
	shader_id = EGGraphics->addShader("post_process", "post_process");
}

void CPostProcessSystem::__update(float fDt)
{
	auto& stage = EGGraphics->getRenderStage("postprocess");
	auto commandBuffer = EGGraphics->getCommandBuffer();
	auto& pShader = EGGraphics->getShader(shader_id);
	auto index = EGGraphics->getDevice()->getCurrentFrame();

	auto& postprocess = EGGraphics->getImage("postprocess_tex_" + std::to_string(index));
	pShader->addTexture("samplerColor", postprocess->getDescriptor());

	auto& pPush = pShader->getPushBlock("ubo");
	pPush->set("enableFXAA", true);
	pPush->set("texelStep", glm::vec2(1.0 / CWindowHandle::iWidth, 1.0 / CWindowHandle::iWidth));
	pPush->flush(commandBuffer);

	auto& pUTonemap = pShader->getUniformBuffer("UBOTonemap");
	pUTonemap->set("gamma", 2.2f);
	pUTonemap->set("exposure", 4.5f);

	auto& pUFXAA = pShader->getUniformBuffer("UBOFXAA");
	pUFXAA->set("qualitySubpix", 0.98f);
	pUFXAA->set("qualityEdgeThreshold", 0.333f);
	pUFXAA->set("qualityEdgeThresholdMin", 0.033f);

	stage->begin(commandBuffer);
	pShader->predraw(commandBuffer);
	commandBuffer.draw(3, 1, 0, 0);

	EGEditor->newFrame();
	stage->end(commandBuffer);
}