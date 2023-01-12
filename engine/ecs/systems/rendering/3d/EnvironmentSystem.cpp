#include "EnvironmentSystem.h"

#include "Engine.h"
#include "graphics/image/Image2D.h"
#include "graphics/image/ImageCubemap.h"

#include "ecs/components/SkyboxComponent.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/CameraComponent.h"

#include "ecs/helper.hpp"

using namespace engine::ecs;
using namespace engine::graphics;

void CEnvironmentSystem::__create()
{
}

void CEnvironmentSystem::__update(float fDt)
{
	auto& registry = EGCoordinator;

	auto commandBuffer = EGGraphics->getCommandBuffer();

	auto ecamera = get_active_camera(registry);
	auto& camera = registry.get<FCameraComponent>(ecamera);
	auto& cameraTransform = registry.get<FTransformComponent>(ecamera);

	auto view = registry.view<FTransformComponent, FSkyboxComponent>();
	for (auto [entity, transform, skybox] : view.each())
	{
		auto& vbo = EGGraphics->getVertexBuffer(skybox.vbo_id);

		vbo->bind(commandBuffer);

		auto& pShader = EGGraphics->getShader(skybox.shader_id);
		if (pShader)
		{
			auto normal = glm::transpose(glm::inverse(transform.model));

			auto& pUBO = pShader->getUniformBuffer("FUniformData");
			pUBO->set("model", transform.model);
			pUBO->set("view", camera.view);
			pUBO->set("projection", camera.projection);
			pUBO->set("normal", normal);
			pUBO->set("viewDir", cameraTransform.rposition);
			pUBO->set("viewportDim", EGGraphics->getDevice()->getExtent(true));
			pUBO->set("frustumPlanes", camera.frustum.getFrustumSides());

			pShader->addTexture("samplerCubeMap", skybox.origin);

			pShader->predraw(commandBuffer);

			commandBuffer.drawIndexed(vbo->getLastIndex(), 1, 0, 0, 0);
		}
	}
}