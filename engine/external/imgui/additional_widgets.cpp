#include "additional_widgets.h"
#include "imgui_internal.h"

bool ImGui::DragDouble(const char* label, double* v, float v_speed, double v_min, double v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalar(label, ImGuiDataType_Double, v, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragDouble2(const char* label, double v[2], float v_speed, double v_min, double v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalarN(label, ImGuiDataType_Float, v, 2, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragDouble3(const char* label, double v[3], float v_speed, double v_min, double v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalarN(label, ImGuiDataType_Float, v, 3, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragDouble4(const char* label, double v[4], float v_speed, double v_min, double v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalarN(label, ImGuiDataType_Float, v, 4, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragDoubleRange2(const char* label, double* v_current_min, double* v_current_max, float v_speed, double v_min, double v_max, const char* format, const char* format_max, ImGuiSliderFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    PushID(label);
    BeginGroup();
    PushMultiItemsWidths(2, CalcItemWidth());

    float min_min = (v_min >= v_max) ? -FLT_MAX : v_min;
    float min_max = (v_min >= v_max) ? *v_current_max : ImMin(v_max, *v_current_max);
    ImGuiSliderFlags min_flags = flags | ((min_min == min_max) ? ImGuiSliderFlags_ReadOnly : 0);
    bool value_changed = DragScalar("##min", ImGuiDataType_Double, v_current_min, v_speed, &min_min, &min_max, format, min_flags);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);

    float max_min = (v_min >= v_max) ? *v_current_min : ImMax(v_min, *v_current_min);
    float max_max = (v_min >= v_max) ? FLT_MAX : v_max;
    ImGuiSliderFlags max_flags = flags | ((max_min == max_max) ? ImGuiSliderFlags_ReadOnly : 0);
    value_changed |= DragScalar("##max", ImGuiDataType_Double, v_current_max, v_speed, &max_min, &max_max, format_max ? format_max : format, max_flags);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    PopID();

    return value_changed;
}


bool ImGui::DragUInt(const char* label, unsigned int* v, float v_speed, unsigned int v_min, unsigned int v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalar(label, ImGuiDataType_U32, v, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragUInt2(const char* label, unsigned int v[2], float v_speed, unsigned int v_min, unsigned int v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalarN(label, ImGuiDataType_U32, v, 2, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragUInt3(const char* label, unsigned int v[3], float v_speed, unsigned int v_min, unsigned int v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalarN(label, ImGuiDataType_U32, v, 3, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragUInt4(const char* label, unsigned int v[4], float v_speed, unsigned int v_min, unsigned int v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalarN(label, ImGuiDataType_U32, v, 4, v_speed, &v_min, &v_max, format, flags);
}

// NB: You likely want to specify the ImGuiSliderFlags_AlwaysClamp when using this.
bool ImGui::DragUIntRange2(const char* label, unsigned int* v_current_min, unsigned int* v_current_max, float v_speed, unsigned int v_min, unsigned int v_max, const char* format, const char* format_max, ImGuiSliderFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    PushID(label);
    BeginGroup();
    PushMultiItemsWidths(2, CalcItemWidth());

    int min_min = (v_min >= v_max) ? INT_MIN : v_min;
    int min_max = (v_min >= v_max) ? *v_current_max : ImMin(v_max, *v_current_max);
    ImGuiSliderFlags min_flags = flags | ((min_min == min_max) ? ImGuiSliderFlags_ReadOnly : 0);
    bool value_changed = DragUInt("##min", v_current_min, v_speed, min_min, min_max, format, min_flags);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);

    int max_min = (v_min >= v_max) ? *v_current_min : ImMax(v_min, *v_current_min);
    int max_max = (v_min >= v_max) ? INT_MAX : v_max;
    ImGuiSliderFlags max_flags = flags | ((max_min == max_max) ? ImGuiSliderFlags_ReadOnly : 0);
    value_changed |= DragUInt("##max", v_current_max, v_speed, max_min, max_max, format_max ? format_max : format, max_flags);
    PopItemWidth();
    SameLine(0, g.Style.ItemInnerSpacing.x);

    TextEx(label, FindRenderedTextEnd(label));
    EndGroup();
    PopID();

    return value_changed;
}
