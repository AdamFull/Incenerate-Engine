#include "Engine.h"
#include "Editor.h"

#include "ecs/systems/systems.h"
#include <SessionStorage.hpp>

#include "EditorRenderSystem.h"

using namespace engine;
using namespace engine::ecs;
using namespace engine::editor;

int main()
{
	CSessionStorage::getInstance()->set("editor_mode", true);
	CSessionStorage::getInstance()->set("display_normals", false);
	CSessionStorage::getInstance()->set("display_physics", false);

	CSessionStorage::getInstance()->set("render_debug_mode", 0);
	CSessionStorage::getInstance()->set("render_debug_cascade_split", 0);
	CSessionStorage::getInstance()->set("render_debug_spot_shadow_index", 0);
	CSessionStorage::getInstance()->set("render_debug_omni_shadow_index", 0);
	CSessionStorage::getInstance()->set("render_debug_omni_shadow_view", 0);

	static int32_t mode;
	static int32_t cascadeSplit;
	static int32_t spotShadowIndex;
	static int32_t omniShadowIndex;
	static int32_t omniShadowView;

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