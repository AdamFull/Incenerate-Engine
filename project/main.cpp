#include "Engine.h"
#include "graphics/image/Image.h"

#include "ecs/systems/systems.h"

#include <SessionStorage.hpp>

using namespace engine;
using namespace engine::ecs;

int main()
{
	CSessionStorage::getInstance()->set("editor_mode", false);
	CSessionStorage::getInstance()->set("display_normals", false);
	CSessionStorage::getInstance()->set("display_physics", false);

	CSessionStorage::getInstance()->set("render_debug_mode", 0);
	CSessionStorage::getInstance()->set("render_debug_cascade_split", 0);
	CSessionStorage::getInstance()->set("render_debug_spot_shadow_index", 0);
	CSessionStorage::getInstance()->set("render_debug_omni_shadow_index", 0);
	CSessionStorage::getInstance()->set("render_debug_omni_shadow_view", 0);

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

	return 0;
}