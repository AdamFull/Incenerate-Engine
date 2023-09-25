#include "EnvironmentSystem.h"

#include "Engine.h"
#include "graphics/image/Image2D.h"
#include "graphics/image/Image3D.h"
#include "graphics/image/ImageCubemap.h"

#include "ecs/components/components.h"

#include "ecs/helper.hpp"
#include <Helpers.h>

using namespace engine::ecs;
using namespace engine::graphics;

void CEnvironmentSystem::__create()
{
	FShaderCreateInfo specials;
	specials.pipeline_stage = "deferred";
	specials.cull_mode = vk::CullModeFlagBits::eBack;
	specials.front_face = vk::FrontFace::eCounterClockwise;
	specials.usages = 1;

	shader_id = graphics->addShader("skybox:skybox", specials);

	CBaseGraphicsSystem::__create();
}

void CEnvironmentSystem::__update(float fDt)
{
	auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	auto atmosphere = graphics->getImageID("sky_atmosphere_texture");

	auto view = registry->view<FTransformComponent, FEnvironmentComponent>();
	for (auto [entity, transform, skybox] : view.each())
	{
		if (skybox.active && skybox.loaded)
		{
			//graphics->bindVertexBuffer(skybox.vbo_id);
			graphics->bindShader(skybox.shader_id);
	
			//auto& pUBO = graphics->getUniformHandle("FUniformData");
			//pUBO->set("model", transform.model);
			//pUBO->set("view", camera->view);
			//pUBO->set("projection", camera->projection);
			//pUBO->set("normal", transform.normal);
			//pUBO->set("viewDir", camera->viewPos);
			//pUBO->set("viewportDim", camera->viewportDim);
			//pUBO->set("frustumPlanes", camera->frustum.getFrustumSides());
			//pUBO->set("object_id", encodeIdToColor(static_cast<uint32_t>(entity)));
			
			graphics->bindTexture("samplerCubeMap", atmosphere);
	
			graphics->flushShader();
			graphics->draw(0, 3, 0, 0, 1);
	
			graphics->bindShader(invalid_index);
			graphics->bindVertexBuffer(invalid_index);
		}
	}
}