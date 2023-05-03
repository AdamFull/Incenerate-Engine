#include "Engine.h"
#include "Editor.h"

#include "ecs/systems/systems.h"

using namespace engine;
using namespace engine::ecs;
using namespace engine::editor;

int main()
{
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
	engine->addSystem<CPresentRenderSystem>();

	engine->create();
	engine->beginEngineLoop();

	//auto editor = std::make_unique<CEditor>();

	return 0;
}