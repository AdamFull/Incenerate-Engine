#include "Engine.h"
#include "Editor.h"

#include "ecs/systems/systems.h"
#include <SessionStorage.hpp>

#include "EditorRenderSystem.h"

using namespace engine;
using namespace engine::ecs;
using namespace engine::editor;

//float calculateOptimalTextureSize(float minExtent, float maxExtent, size_t pixelSize, size_t textureCount, size_t memoryBudget, float quality) 
//{
//	// Calculate available budget
//	size_t availableBudget = static_cast<size_t>(memoryBudget * quality);
//
//	// Start with the max extent and work our way down
//	for (float extent = maxExtent; extent >= minExtent; extent /= 2) 
//	{
//		size_t requiredMemory = static_cast<size_t>(std::pow(extent, 2) * pixelSize * textureCount);
//
//		// Check if the texture fits in the available budget
//		if (requiredMemory <= availableBudget)
//			return extent;
//	}
//
//	// If no suitable extent was found, return the minimum extent
//	return minExtent;
//}

int main()
{
	//auto budget = 4ull * 1024ull * 1024ull * 1024ull;
	//auto tex_size = calculateOptimalTextureSize(512.f, 8192.f, 4, 8, budget, 0.15);

	CSessionStorage::getInstance()->set("editor_mode", true);
	CSessionStorage::getInstance()->set("display_normals", false);
	CSessionStorage::getInstance()->set("display_physics", false);

	CSessionStorage::getInstance()->set("render_debug_mode", 0);
	CSessionStorage::getInstance()->set("render_debug_cascade_split", 0);
	CSessionStorage::getInstance()->set("render_debug_spot_shadow_index", 0);
	CSessionStorage::getInstance()->set("render_debug_omni_shadow_index", 0);
	CSessionStorage::getInstance()->set("render_debug_omni_shadow_view", 0);

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