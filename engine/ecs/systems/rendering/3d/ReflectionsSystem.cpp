#include "ReflectionsSystem.h"

#include "Engine.h"

#include "graphics/GraphicsSettings.h"

#include "ecs/components/CameraComponent.h"

using namespace engine::ecs;
using namespace engine::graphics;

void CReflectionsSystem::__create()
{
	FShaderCreateInfo specials;
	specials.pipeline_stage = "ssr";
	specials.cull_mode = vk::CullModeFlagBits::eBack;
	specials.usages = 1;

	shader_id = graphics->addShader("effects:ssr", specials);

	addSubresource("composition_tex");
	addSubresource("global_illumination_tex");
	addSubresource("normal_tex");
	addSubresource("mrah_tex");
	addSubresource("depth_tex");

	CBaseGraphicsSystem::__create();
}

void CReflectionsSystem::__update(float fDt)
{
	auto& settings = CGraphicsSettings::getInstance()->getSettings();

	auto* camera = EGEngine->getActiveCamera();

	if (!camera || !settings.bEnableReflections)
		return;

	auto projection = camera->projection;
	projection[1][1] *= -1.f;

	size_t color_texture = settings.bEnableGlobalIllumination ? getSubresource("global_illumination_tex") : getSubresource("composition_tex");

	auto stage = graphics->getRenderStageID("ssr");

	graphics->bindShader(shader_id);

	graphics->bindTexture("depth_tex", getSubresource("depth_tex"));
	graphics->bindTexture("mrah_tex", getSubresource("mrah_tex"));
	graphics->bindTexture("normal_tex", getSubresource("normal_tex"));
	graphics->bindTexture("albedo_tex", color_texture);

	auto& pUBO = graphics->getUniformHandle("UBOGeneralMatrices");
	//auto& pUBO = graphics->getPushBlockHandle("data");
	pUBO->set("projection", projection);
	pUBO->set("invProjection", glm::inverse(projection));
	pUBO->set("view", camera->view);
	pUBO->set("invView", glm::inverse(camera->view));
	//graphics->flushConstantRanges(pUBO);

	auto& pReflections = graphics->getUniformHandle("UBOReflections");
	pReflections->set("rayStep", settings.fReflectionRayStep);
	pReflections->set("iterationCount", settings.iReflectionIterationCount);
	pReflections->set("distanceBias", settings.fReflectionDistanceBias);
	pReflections->set("sampleCount", settings.iReflectionSampleCount);
	pReflections->set("samplingCoefficient", settings.fReflectionSamplingCoefficient);
	pReflections->set("reflectionSpecularFalloffExponent", settings.fReflectionSpecularFalloffExponent);
	pReflections->set("debugDraw", settings.bReflectionDebugDraw ? 1 : -1);
	pReflections->set("isBinarySearchEnabled", settings.bReflectionBinarySearchEnabled ? 1 : -1);
	pReflections->set("isAdaptiveStepEnabled", settings.bReflectionAdaptiveStepEnabled ? 1 : -1);
	pReflections->set("isExponentialStepEnabled", settings.bReflectionExponentialStepEnabled ? 1 : -1);
	pReflections->set("isSamplingEnabled", settings.bReflectionSamplingEnabled ? 1 : -1);

	graphics->bindRenderer(stage);
	graphics->flushShader();
	graphics->draw(0, 3, 0, 0, 1);
	graphics->bindRenderer(invalid_index);
}