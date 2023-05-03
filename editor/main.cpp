#include "Engine.h"
#include "Editor.h"

#include "ecs/systems/systems.h"
#include "EditorRenderSystem.h"

using namespace engine;
using namespace engine::ecs;
using namespace engine::editor;

int main()
{
	auto& editor = CEditor::getInstance();
	auto& engine = CEngine::getInstance();

	engine->initialize();

	engine->addSystem<CAnimationSystem>();
	engine->addSystem<CHierarchySystem>();
	engine->addSystem<CPhysicsSystem>();

	engine->addSystem<CInputSystem>();
	engine->addSystem<CScriptingSystem>();
	engine->addSystem<CCameraControlSystem>();
	engine->addSystem<CAudioSystem>();
	engine->addSystem<C3DRenderSystem>();
	engine->addSystem<CEditorRenderSystem>();

	engine->create();
	editor->create();

	engine->beginEngineLoop();

	return 0;
}