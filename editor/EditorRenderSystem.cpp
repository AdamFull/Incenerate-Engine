#include "EditorRenderSystem.h"

#include "Editor.h"
#include "Engine.h"

using namespace engine::editor;

void CEditorRenderSystem::__create()
{
	graphics = EGEngine->getGraphics().get();
}

void CEditorRenderSystem::__update(float fDt)
{
	auto stage = graphics->getRenderStageID("present");

	graphics->bindRenderer(stage);
	EGEditor->newFrame(fDt);
	graphics->bindRenderer(invalid_index);
}