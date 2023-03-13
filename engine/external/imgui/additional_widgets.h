#pragma once

#include "imgui.h"

namespace ImGui
{
    IMGUI_API bool DragDouble(const char* label, double* v, float v_speed = 1.0f, double v_min = 0.0f, double v_max = 0.0f, const char* format = "%.3lf", ImGuiSliderFlags flags = 0);     // If v_min >= v_max we have no bound
    IMGUI_API bool DragDouble2(const char* label, double v[2], float v_speed = 1.0f, double v_min = 0.0f, double v_max = 0.0f, const char* format = "%.3lf", ImGuiSliderFlags flags = 0);
    IMGUI_API bool DragDouble3(const char* label, double v[3], float v_speed = 1.0f, double v_min = 0.0f, double v_max = 0.0f, const char* format = "%.3lf", ImGuiSliderFlags flags = 0);
    IMGUI_API bool DragDouble4(const char* label, double v[4], float v_speed = 1.0f, double v_min = 0.0f, double v_max = 0.0f, const char* format = "%.3lf", ImGuiSliderFlags flags = 0);
    IMGUI_API bool DragDoubleRange2(const char* label, double* v_current_min, double* v_current_max, float v_speed = 1.0f, double v_min = 0.0f, double v_max = 0.0f, const char* format = "%.3lf", const char* format_max = NULL, ImGuiSliderFlags flags = 0);
    IMGUI_API bool DragUInt(const char* label, unsigned int* v, float v_speed = 1.0f, unsigned int v_min = 0, unsigned int v_max = 0, const char* format = "%u", ImGuiSliderFlags flags = 0);  // If v_min >= v_max we have no bound
    IMGUI_API bool DragUInt2(const char* label, unsigned int v[2], float v_speed = 1.0f, unsigned int v_min = 0, unsigned int v_max = 0, const char* format = "%u", ImGuiSliderFlags flags = 0);
    IMGUI_API bool DragUInt3(const char* label, unsigned int v[3], float v_speed = 1.0f, unsigned int v_min = 0, unsigned int v_max = 0, const char* format = "%u", ImGuiSliderFlags flags = 0);
    IMGUI_API bool DragUInt4(const char* label, unsigned int v[4], float v_speed = 1.0f, unsigned int v_min = 0, unsigned int v_max = 0, const char* format = "%u", ImGuiSliderFlags flags = 0);
    IMGUI_API bool DragUIntRange2(const char* label, unsigned int* v_current_min, unsigned int* v_current_max, float v_speed = 1.0f, unsigned int v_min = 0, unsigned int v_max = 0, const char* format = "%u", const char* format_max = NULL, ImGuiSliderFlags flags = 0);
}
