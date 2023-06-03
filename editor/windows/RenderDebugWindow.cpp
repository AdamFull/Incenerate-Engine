#include "RenderDebugWindow.h"

#include <imgui/imgui.h>
#include "CustomControls.h"

#include <SessionStorage.hpp>

using namespace engine::editor;

const std::array<const char*, 13> debug_render_mode{ "default", "positions", "depth", "albedo", "normal", "emission", "roughness", "metalness", "ao", "ao strength", "reflections", "global_illumination", "csm_cascades"};

void CRenderDebugWindow::create()
{

}

void CRenderDebugWindow::__draw(float fDt)
{
	auto mode = CSessionStorage::getInstance()->get<int32_t>("render_debug_mode");
	if(ImGui::GCombo("Display", &mode, debug_render_mode.data(), debug_render_mode.size()))
		CSessionStorage::getInstance()->set("render_debug_mode", mode);
}