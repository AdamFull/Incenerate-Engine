#include "EnvironmentSystem.h"

#include "Engine.h"
#include "graphics/image/Image2D.h"
#include "graphics/image/ImageCubemap.h"

#include "ecs/components/components.h"

#include "ecs/helper.hpp"

using namespace engine::ecs;
using namespace engine::graphics;

void CEnvironmentSystem::__create()
{
	CBaseGraphicsSystem::__create();
}

void CEnvironmentSystem::__update(float fDt)
{
	auto& registry = EGCoordinator;
	auto editorMode = EGEngine->isEditorMode();
	auto state = EGEngine->getState();

	auto commandBuffer = EGGraphics->getCommandBuffer();

	FCameraComponent* camera{ nullptr };

	if (editorMode && state == EEngineState::eEditing)
		camera = registry.try_get<FCameraComponent>(EGEditor->getCamera());
	else
		camera = registry.try_get<FCameraComponent>(get_active_camera(registry));

	if (camera)
	{
		auto view = registry.view<FTransformComponent, FEnvironmentComponent>();
		for (auto [entity, transform, skybox] : view.each())
		{
			if (skybox.active && skybox.loaded)
			{
				auto& vbo = EGGraphics->getVertexBuffer(skybox.vbo_id);

				vbo->bind(commandBuffer);

				auto& pShader = EGGraphics->getShader(skybox.shader_id);
				if (pShader)
				{
					auto normal = glm::transpose(glm::inverse(transform.model));

					auto& pUBO = pShader->getUniformBuffer("FUniformData");
					pUBO->set("model", transform.model);
					pUBO->set("view", camera->view);
					pUBO->set("projection", camera->projection);
					pUBO->set("normal", normal);
					pUBO->set("viewDir", camera->viewPos);
					pUBO->set("viewportDim", EGGraphics->getDevice()->getExtent(true));
					pUBO->set("frustumPlanes", camera->frustum.getFrustumSides());

					pShader->addTexture("samplerCubeMap", skybox.origin);

					pShader->predraw(commandBuffer);

					commandBuffer.drawIndexed(vbo->getLastIndex(), 1, 0, 0, 0);
				}
			}
		}
	}
}