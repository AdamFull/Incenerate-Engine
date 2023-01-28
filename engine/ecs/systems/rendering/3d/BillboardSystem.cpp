#include "BillboardSystem.h"

#include "Engine.h"

#include "ecs/components/components.h"
#include "ecs/helper.hpp"

using namespace engine::ecs;

void CBillboardSystem::__create()
{
	shader_id = EGGraphics->addShader("billboard", "billboard");
}

void CBillboardSystem::__update(float fDt)
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

	graphics->bindShader(shader_id);
	graphics->setManualShaderControlFlag(true);

	auto& pPush = graphics->getPushBlockHandle("billboard");

	{
		auto view = registry.view<FTransformComponent, FPointLightComponent>();
		for (auto [entity, transform, light] : view.each())
		{
			pPush->set("projection", camera->projection);
			pPush->set("view", camera->view);
			pPush->set("cameraPosition", camera->viewPos);
			pPush->set("billboardPosition", transform.rposition);
			graphics->flushConstantRanges(pPush);

			graphics->draw(0, 6, 0, 0, 1);
		}
	}

	graphics->setManualShaderControlFlag(false);
	graphics->bindShader(invalid_index);
}