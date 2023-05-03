#include "RenderDebugWindow.h"

#include <imgui/imgui.h>
#include "CustomControls.h"

#include "DebugGlobals.h"

using namespace engine::editor;

const std::array<const char*, 13> debug_render_mode{ "default", "positions", "depth", "albedo", "normal", "emission", "roughness", "metalness", "ao", "ao strength", "csm", "dsm", "osm" };

void CRenderDebugWindow::create()
{

}

void CRenderDebugWindow::__draw(float fDt)
{
	ImGui::GCombo("Display", &FDebugGlobal::mode, debug_render_mode.data(), debug_render_mode.size());

	// Cascade shadow map debug
	if (FDebugGlobal::mode == 10)
		ImGui::GSliderInt("Split", &FDebugGlobal::cascadeSplit, 0, SHADOW_MAP_CASCADE_COUNT);
	// Spot shadow map debug
	else if(FDebugGlobal::mode == 11)
		ImGui::GSliderInt("Index", &FDebugGlobal::spotShadowIndex, 0, MAX_SPOT_LIGHT_COUNT);
	// Omni shadow map debug
	else if (FDebugGlobal::mode == 12)
	{
		ImGui::GSliderInt("View", &FDebugGlobal::omniShadowView, 0, 6);
		ImGui::GSliderInt("Index", &FDebugGlobal::omniShadowIndex, 0, MAX_POINT_LIGHT_COUNT);
	}
}