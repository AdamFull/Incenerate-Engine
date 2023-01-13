#include "EditorThemes.h"


#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

using namespace engine::editor;

void Themes::standart()
{
    // Color scheme
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 5.3f;
    style.PopupRounding = style.TabRounding = style.GrabRounding = style.FrameRounding = 2.3f;
    style.ScrollbarRounding = 5.0f;
    style.FrameBorderSize = 1.0f;
    style.ItemSpacing.y = 6.5f;
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Text] = ImVec4{ 0.73333335f, 0.73333335f, 0.73333335f, 1.00f };
    colors[ImGuiCol_TextDisabled] = ImVec4{ 0.34509805f, 0.34509805f, 0.34509805f, 1.00f };
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.23529413f, 0.24705884f, 0.25490198f, 0.94f };
    colors[ImGuiCol_ChildBg] = ImVec4{ 0.23529413f, 0.24705884f, 0.25490198f, 0.00f };
    colors[ImGuiCol_PopupBg] = ImVec4{ 0.23529413f, 0.24705884f, 0.25490198f, 0.94f };
    colors[ImGuiCol_Border] = ImVec4{ 0.33333334f, 0.33333334f, 0.33333334f, 0.50f };
    colors[ImGuiCol_BorderShadow] = ImVec4{ 0.15686275f, 0.15686275f, 0.15686275f, 0.00f };
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.16862746f, 0.16862746f, 0.16862746f, 0.54f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.453125f, 0.67578125f, 0.99609375f, 0.67f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.47058827f, 0.47058827f, 0.47058827f, 0.67f };
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.04f, 0.04f, 0.04f, 1.00f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.16f, 0.29f, 0.48f, 1.00f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.51f };
    colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.27058825f, 0.28627452f, 0.2901961f, 0.80f };
    colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.27058825f, 0.28627452f, 0.2901961f, 0.60f };
    colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.21960786f, 0.30980393f, 0.41960788f, 0.51f };
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.21960786f, 0.30980393f, 0.41960788f, 1.00f };
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.13725491f, 0.19215688f, 0.2627451f, 0.91f };
    // style->Colors[ImGuiCol_ComboBg]               = {0.1f, 0.1f, 0.1f, 0.99f};
    colors[ImGuiCol_CheckMark] = ImVec4{ 0.90f, 0.90f, 0.90f, 0.83f };
    colors[ImGuiCol_SliderGrab] = ImVec4{ 0.70f, 0.70f, 0.70f, 0.62f };
    colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.30f, 0.30f, 0.30f, 0.84f };
    colors[ImGuiCol_Button] = ImVec4{ 0.33333334f, 0.3529412f, 0.36078432f, 0.49f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.21960786f, 0.30980393f, 0.41960788f, 1.00f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.13725491f, 0.19215688f, 0.2627451f, 1.00f };
    colors[ImGuiCol_Header] = ImVec4{ 0.33333334f, 0.3529412f, 0.36078432f, 0.53f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.453125f, 0.67578125f, 0.99609375f, 0.67f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.47058827f, 0.47058827f, 0.47058827f, 0.67f };
    colors[ImGuiCol_Separator] = ImVec4{ 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
    colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
    colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
    colors[ImGuiCol_ResizeGrip] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.85f };
    colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.60f };
    colors[ImGuiCol_ResizeGripActive] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.90f };
    colors[ImGuiCol_PlotLines] = ImVec4{ 0.61f, 0.61f, 0.61f, 1.00f };
    colors[ImGuiCol_PlotLinesHovered] = ImVec4{ 1.00f, 0.43f, 0.35f, 1.00f };
    colors[ImGuiCol_PlotHistogram] = ImVec4{ 0.90f, 0.70f, 0.00f, 1.00f };
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4{ 1.00f, 0.60f, 0.00f, 1.00f };
    colors[ImGuiCol_TextSelectedBg] = ImVec4{ 0.18431373f, 0.39607847f, 0.79215693f, 0.90f };
}

void Themes::cinder()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.TabRounding = 0.0f;
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 0.50f);
    colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.92f, 0.18f, 0.29f, 0.76f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.9f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);

    colors[ImGuiCol_Separator] = colors[ImGuiCol_MenuBarBg];
    colors[ImGuiCol_SeparatorActive] = colors[ImGuiCol_Separator];
    colors[ImGuiCol_SeparatorHovered] = colors[ImGuiCol_Separator];

    colors[ImGuiCol_Tab] = colors[ImGuiCol_Button];
    colors[ImGuiCol_TabHovered] = colors[ImGuiCol_ButtonHovered];
    colors[ImGuiCol_TabActive] = colors[ImGuiCol_ButtonActive];
    colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
}