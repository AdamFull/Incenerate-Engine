#include "GlobalIlluminationSystem.h"
#include "image/Image.h"
#include "Engine.h"

#include "Helpers.h"

#include "image/Image2D.h"
#include "graphics/GraphicsSettings.h"

#include "ecs/components/CameraComponent.h"

#include <random>

using namespace engine::ecs;
using namespace engine::graphics;



void CGlobalIlluminationSystem::__create()
{
	std::default_random_engine rndEngine(0);
	std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

	{
		FShaderSpecials specials;
		specials.defines =
		{
			{"SSDO_KERNEL_SIZE", std::to_string(SSAO_KERNEL_SIZE)}
		};
		shader_id = graphics->addShader("ssdo_pass", specials);
	}

	for (uint32_t i = 0; i < SSAO_KERNEL_SIZE; ++i)
	{
		glm::vec3 sample(rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine));
		sample = glm::normalize(sample);
		sample *= rndDist(rndEngine);
		float scale = float(i) / float(SSAO_KERNEL_SIZE);
		scale = lerp(0.1f, 1.0f, scale * scale);
		vSSDOKernel[i] = glm::vec4(sample * scale, 0.0f);
	}

	std::array<glm::vec4, SSAO_NOISE_DIM* SSAO_NOISE_DIM> vSSDONoise;
	for (uint32_t i = 0; i < static_cast<uint32_t>(vSSDONoise.size()); i++)
		vSSDONoise[i] = glm::vec4(rndDist(rndEngine) * 2.0f - 1.0f, rndDist(rndEngine) * 2.0f - 1.0f, 0.0f, 0.0f);
	
	// Create noise image
	auto pNoise = std::make_unique<CImage>(graphics->getDevice().get());
	pNoise->create(vSSDONoise.data(), vk::Extent3D{ SSAO_NOISE_DIM, SSAO_NOISE_DIM, sizeof(glm::vec4) }, vk::Format::eR32G32B32A32Sfloat);
	
	noise_image = graphics->addImage("ssdo_noise_image", std::move(pNoise));

	addSubresource("albedo_tex");
	addSubresource("depth_tex");

	CBaseGraphicsSystem::__create();
}

void CGlobalIlluminationSystem::__update(float fDt)
{
	auto& settings = CGraphicsSettings::getInstance()->getSettings();

	auto* camera = EGEngine->getActiveCamera();

	if (!camera || !settings.bEnableGlobalIllumination)
		return;

	auto projection = camera->projection;
	projection[1][1] *= -1.f;

	auto stage = graphics->getRenderStageID("global_illumination");

	graphics->bindShader(shader_id);

	graphics->bindTexture("albedo_tex", getSubresource("albedo_tex"));
	graphics->bindTexture("depth_tex", getSubresource("depth_tex"));
	graphics->bindTexture("ssdo_noise_tex", noise_image);

	auto& pUBO = graphics->getUniformHandle("UBODirectionalOcclusion");
	pUBO->set("samples", vSSDOKernel);
	pUBO->set("invProjection", glm::inverse(projection));
	pUBO->set("projection", projection);
	pUBO->set("zNear", camera->nearPlane);
	pUBO->set("zFar", camera->farPlane);
	pUBO->set("radius", settings.fAmbientOcclusionRadius);
	pUBO->set("bias", settings.fAmbientOcclusionBias);

	graphics->bindRenderer(stage);
	graphics->flushShader();
	graphics->draw(0, 3, 0, 0, 1);
	graphics->bindRenderer(invalid_index);
}