#include "EnvironmentSystem.h"

#include "Engine.h"
#include "graphics/image/Image2D.h"
#include "graphics/image/ImageCubemap.h"

#include "ecs/components/components.h"

#include "ecs/helper.hpp"
#include <Helpers.h>

using namespace engine::ecs;
using namespace engine::graphics;

void CEnvironmentSystem::__create()
{
	FShaderSpecials specials;
	specials.defines = {
		{"MAX_DIRECTIONAL_LIGHTS_COUNT", std::to_string(MAX_DIRECTIONAL_LIGHT_COUNT)}
	};
	shader_id = graphics->addShader("new_skybox", specials);

	CBaseGraphicsSystem::__create();
}

void CEnvironmentSystem::__update(float fDt)
{
	uint32_t directoonal_light_count{ 0 };
	static std::array<FDirectionalLightCommit, MAX_DIRECTIONAL_LIGHT_COUNT> directional_lights;

	auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	//auto view = registry->view<FTransformComponent, FEnvironmentComponent>();
	//for (auto [entity, transform, skybox] : view.each())
	//{
	//	if (skybox.active && skybox.loaded)
	//	{
	//		graphics->bindVertexBuffer(skybox.vbo_id);
	//		graphics->bindShader(skybox.shader_id);
	//
	//		auto& pUBO = graphics->getUniformHandle("FUniformData");
	//		pUBO->set("model", transform.model);
	//		pUBO->set("view", camera->view);
	//		pUBO->set("projection", camera->projection);
	//		pUBO->set("normal", transform.normal);
	//		pUBO->set("viewDir", camera->viewPos);
	//		pUBO->set("viewportDim", camera->viewportDim);
	//		pUBO->set("frustumPlanes", camera->frustum.getFrustumSides());
	//		pUBO->set("object_id", encodeIdToColor(static_cast<uint32_t>(entity)));
	//
	//		graphics->bindTexture("samplerCubeMap", skybox.origin);
	//
	//		graphics->flushShader();
	//		graphics->draw();
	//
	//		graphics->bindShader(invalid_index);
	//		graphics->bindVertexBuffer(invalid_index);
	//	}
	//}

	// Collecting directional lights
	{
		auto view = registry->view<FTransformComponent, FDirectionalLightComponent>();
		for (auto [entity, transform, light] : view.each())
		{
			if (directoonal_light_count > MAX_DIRECTIONAL_LIGHT_COUNT)
				break;

			FDirectionalLightCommit commit;
			commit.direction = glm::normalize(glm::toQuat(transform.model) * glm::vec3(0.f, 0.f, 1.f));
			commit.color = light.color;
			commit.intencity = light.intencity;
			commit.shadowIndex = light.shadowIndex;
			commit.castShadows = static_cast<int>(light.castShadows);

			directional_lights[directoonal_light_count++] = commit;
		}
	}

	auto invView = glm::inverse(camera->view);
	auto invProjection = glm::inverse(camera->projection);
	auto invViewProjection = glm::inverse(camera->projection * camera->view);

	graphics->bindShader(shader_id);
	auto& pCamera = graphics->getUniformHandle("FCameraData");
	pCamera->set("view", camera->view);
	pCamera->set("invView", invView);
	pCamera->set("projection", camera->projection);
	pCamera->set("invProjection", invProjection);
	pCamera->set("invViewProjection", invViewProjection);

	auto& pScattering = graphics->getUniformHandle("FAtmosphereScattering");
	pScattering->set("lightCount", directoonal_light_count);

	auto& pLights = graphics->getUniformHandle("UBOLights");
	pLights->set("directionalLights", directional_lights);

	graphics->flushShader();
	graphics->draw(0, 3, 0, 0, 1);
	
	graphics->bindShader(invalid_index);
	graphics->bindVertexBuffer(invalid_index);
}