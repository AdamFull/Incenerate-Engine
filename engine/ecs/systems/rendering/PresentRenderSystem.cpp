#include "PresentRenderSystem.h"

#include "Engine.h"

using namespace engine::ecs;

void CPresentRenderSystem::__create()
{
	if (EGEngine->isEditorMode())
		EGEditor->create();

	shader_id = graphics->addShader("through_pass", "through_pass");
}

void CPresentRenderSystem::__update(float fDt)
{
	auto stage = graphics->getRenderStageID("present");
	auto present_id = graphics->getImageID("postprocess_tex");

	graphics->bindRenderer(stage);

	if(EGEngine->isEditorMode())
		EGEditor->newFrame(fDt);
	else
	{
		graphics->bindShader(shader_id);

		graphics->bindTexture("samplerColor", present_id);;
		graphics->draw(0, 3, 0, 0, 1);

		graphics->bindShader(invalid_index);
	}

	graphics->bindRenderer(invalid_index);
}