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
	auto& graphics = EGGraphics;
	auto editorMode = EGEngine->isEditorMode();
	auto state = EGEngine->getState();

	FCameraComponent* camera{ nullptr };

	if (editorMode && state == EEngineState::eEditing)
		camera = registry.try_get<FCameraComponent>(EGEditor->getCamera());
	else
		camera = registry.try_get<FCameraComponent>(get_active_camera(registry));

	if (!camera)
		return;

	auto view = registry.view<FTransformComponent, FEnvironmentComponent>();
	for (auto [entity, transform, skybox] : view.each())
	{
		if (skybox.active && skybox.loaded)
		{
			graphics->bindVertexBuffer(skybox.vbo_id);
			graphics->bindShader(skybox.shader_id);

			auto normal = glm::transpose(glm::inverse(transform.model));

			auto& pUBO = graphics->getUniformHandle("FUniformData");
			pUBO->set("model", transform.model);
			pUBO->set("view", camera->view);
			pUBO->set("projection", camera->projection);
			pUBO->set("normal", normal);
			pUBO->set("viewDir", camera->viewPos);
			pUBO->set("viewportDim", camera->viewportDim);
			pUBO->set("frustumPlanes", camera->frustum.getFrustumSides());

			graphics->bindTexture("samplerCubeMap", skybox.origin);

			graphics->draw();

			graphics->bindShader(invalid_index);
			graphics->bindVertexBuffer(invalid_index);
		}
	}
}