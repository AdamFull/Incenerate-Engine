#include "RenderDebugWindow.h"

#include <imgui/imgui.h>
#include "CustomControls.h"

#include <SessionStorage.hpp>

using namespace engine::editor;

const std::array<const char*, 12> debug_render_mode{ "default", "positions", "depth", "albedo", "normal", "emission", "roughness", "metalness", "ao", "ao strength", "csm", "csm_cascades"};

void CRenderDebugWindow::create()
{

}

void CRenderDebugWindow::__draw(float fDt)
{
	auto mode = CSessionStorage::getInstance()->get<int32_t>("render_debug_mode");
	if(ImGui::GCombo("Display", &mode, debug_render_mode.data(), debug_render_mode.size()))
		CSessionStorage::getInstance()->set("render_debug_mode", mode);

	

	// Cascade shadow map debug
	if (mode == 10)
	{
		auto split = CSessionStorage::getInstance()->get<int32_t>("render_debug_cascade_split");
		if (ImGui::GSliderInt("Split", &split, 0, SHADOW_MAP_CASCADE_COUNT))
			CSessionStorage::getInstance()->set("render_debug_cascade_split", split);
	}
		
	//// Spot shadow map debug
	//else if (mode == 11)
	//{
	//	auto index = CSessionStorage::getInstance()->get<int32_t>("render_debug_spot_shadow_index");
	//	if(ImGui::GSliderInt("Index", &index, 0, MAX_SPOT_LIGHT_COUNT))
	//		CSessionStorage::getInstance()->set("render_debug_spot_shadow_index", index);
	//}
	//	
	//// Omni shadow map debug
	//else if (mode == 12)
	//{
	//	auto view = CSessionStorage::getInstance()->get<int32_t>("render_debug_omni_shadow_view");
	//	if(ImGui::GSliderInt("View", &view, 0, 6))
	//		CSessionStorage::getInstance()->set("render_debug_omni_shadow_view", view);
	//
	//	auto index = CSessionStorage::getInstance()->get<int32_t>("render_debug_omni_shadow_index");
	//	if(ImGui::GSliderInt("Index", &index, 0, MAX_POINT_LIGHT_COUNT))
	//		CSessionStorage::getInstance()->set("render_debug_omni_shadow_index", index);
	//}
}