#include "EditorRenderSystem.h"

#include "Engine.h"

using namespace engine::ecs;

void CEditorRenderSystem::__create()
{
	EGEditor->create();
}

void CEditorRenderSystem::__update(float fDt)
{
	auto commandBuffer = EGGraphics->getCommandBuffer();
	auto& stage = EGGraphics->getRenderStage("present");

	stage->begin(commandBuffer);

	EGEditor->newFrame(fDt);

	stage->end(commandBuffer);
}