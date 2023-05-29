#include "Engine.h"
#include "Editor.h"

#include "ecs/systems/systems.h"
#include <SessionStorage.hpp>

#include "EditorRenderSystem.h"

using namespace engine;
using namespace engine::ecs;
using namespace engine::editor;

//size_t calculateOptimalTextureSize(size_t minSize, size_t maxSize, size_t pixelWeight, size_t memoryBudget, size_t desiredQuality) {
//	// Calculate the number of pixels that can fit into the memory budget
//	size_t pixelBudget = memoryBudget / pixelWeight;
//
//	// Define the boundaries for the texture sizes
//	size_t lowerBound = std::log2(minSize);
//	size_t upperBound = std::log2(maxSize);
//
//	// Map the desiredQuality to a range between lowerBound and upperBound
//	size_t range = upperBound - lowerBound;
//	size_t optimalSizeLog2 = lowerBound + static_cast<size_t>(std::round(range * (static_cast<double>(desiredQuality) / 4.0)));
//
//	// Check if the optimal texture size fits into the memory budget
//	while ((std::pow(2, optimalSizeLog2) * std::pow(2, optimalSizeLog2) * pixelWeight) > memoryBudget && optimalSizeLog2 > lowerBound) {
//		// If it does not fit, decrease the texture size until it fits into the budget
//		optimalSizeLog2--;
//	}
//
//	// Return the optimal texture size as a power of two
//	return std::pow(2, optimalSizeLog2);
//}

int main()
{
	//auto tex_size = calculateOptimalTextureSize(512, 8192, 4, 4194304, 1000);

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