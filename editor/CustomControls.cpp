#include "CustomControls.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <imgui/additional_widgets.h>

#include <tinyfiledialogs/tinyfiledialogs.h>

#include "Editor.h"
#include "operations/PropertyChangedOperation.h"

using namespace engine;
using namespace engine::editor;

constexpr const float _columnWidth{100.f};

template<class _Ty>
void applyPropertyChange(_Ty& vold, _Ty* vnew)
{
    if (ImGui::IsItemActivated())
        vold = *vnew;

    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        auto& actionBuffer = EGEditor->getActionBuffer();
        actionBuffer->addOperation(std::make_unique<CPropertyChangedOperation>(vold, vnew, utl::type_hash<_Ty>()));
    }
}

template<class _Ty, class _Func>
bool SimpleValueEdit(const char* label, _Ty* value, _Func&& function)
{
    static _Ty oldValue{};
    bool bValueChanged{ false };

    ImGui::PushID(label);

    if (ImGui::BeginTable("data_table", 2, ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableSetupColumn("Parameter", 0, ImGui::GetContentRegionAvail().x * ImGui::widthScale);
        ImGui::TableSetupColumn("Field", 0, ImGui::GetContentRegionAvail().x * (1.f - ImGui::widthScale));

        ImGui::TableNextColumn();
        ImGui::Text(label);

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-1.f);
        bValueChanged = function("##input", value);
        applyPropertyChange(oldValue, value);

        ImGui::EndTable();
    }

    ImGui::PopID();

    return bValueChanged;
}

template<class _Ty, class _Func>
bool DragScalarExt(const char* label, _Ty* value, _Ty step, _Ty min, _Ty max, _Func&& function)
{
    static _Ty oldValue{ 0 };

    bool bValueChanged{ false };
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label);

    if (ImGui::BeginTable("data_table", 2, ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableSetupColumn("Parameter", 0, ImGui::GetContentRegionAvail().x * ImGui::widthScale);
        ImGui::TableSetupColumn("Field", 0, ImGui::GetContentRegionAvail().x * (1.f - ImGui::widthScale));

        ImGui::TableNextColumn();
        ImGui::Text(label);

        ImGui::TableNextColumn();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        float lineHeight = boldFont->FontSize + 6.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("N", buttonSize))
        {
            bValueChanged = true;
            *value = min;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        bValueChanged = function("##N", value, step, min, max);
        applyPropertyChange(oldValue, value);

        ImGui::PopStyleVar();

        ImGui::EndTable();
    }

    ImGui::PopID();

    return bValueChanged;
}

template<class _Ty, class _Func>
bool DragVector3Ext(const char* label, _Ty value[3], _Ty step, _Ty min, _Ty max, _Func&& function)
{
    static _Ty oldValue[3]{ 0 };

    bool bValueChanged{ false };
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label);

    if (ImGui::BeginTable("data_table", 2, ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableSetupColumn("Parameter", 0, ImGui::GetContentRegionAvail().x * ImGui::widthScale);
        ImGui::TableSetupColumn("Field", 0, ImGui::GetContentRegionAvail().x * (1.f - ImGui::widthScale));

        ImGui::TableNextColumn();
        ImGui::Text(label);

        ImGui::TableNextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::GetContentRegionAvail().x * (1.f - ImGui::widthScale));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("X"))
        {
            bValueChanged = true;
            value[0] = min;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        bValueChanged |= function("##X", &value[0], step, min, max);
        applyPropertyChange(oldValue[0], &value[0]);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y"))
        {
            bValueChanged = true;
            value[1] = min;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        bValueChanged |= function("##Y", &value[1], step, min, max);
        applyPropertyChange(oldValue[1], &value[1]);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Z"))
        {
            bValueChanged = true;
            value[2] = min;
        }

        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        bValueChanged |= function("##Z", &value[2], step, min, max);
        applyPropertyChange(oldValue[2], &value[2]);
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::EndTable();
    }

    ImGui::PopID();
    return bValueChanged;
}

template<class _Ty, class _Func>
bool DragVector4Ext(const char* label, _Ty value[4], _Ty step, _Ty min, _Ty max, _Func&& function)
{
    static _Ty oldValue[4]{ 0 };

    bool bValueChanged{ false };
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label);

    if (ImGui::BeginTable("data_table", 2, ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableSetupColumn("Parameter", 0, ImGui::GetContentRegionAvail().x * ImGui::widthScale);
        ImGui::TableSetupColumn("Field", 0, ImGui::GetContentRegionAvail().x * (1.f - ImGui::widthScale));

        ImGui::TableNextColumn();
        ImGui::Text(label);

        ImGui::TableNextColumn();

        ImGui::PushMultiItemsWidths(4, ImGui::GetContentRegionAvail().x * (1.f - ImGui::widthScale));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("X"))
        {
            bValueChanged = true;
            value[0] = min;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        bValueChanged |= function("##X", &value[0], step, min, max);
        applyPropertyChange(oldValue[0], &value[0]);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y"))
        {
            bValueChanged = true;
            value[1] = min;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        bValueChanged |= function("##Y", &value[1], step, min, max);
        applyPropertyChange(oldValue[1], &value[1]);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Z"))
        {
            bValueChanged = true;
            value[2] = min;
        }

        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        bValueChanged |= function("##Z", &value[2], step, min, max);
        applyPropertyChange(oldValue[2], &value[2]);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.6f, 0.6f, 0.6f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("W"))
        {
            bValueChanged = true;
            value[3] = min;
        }

        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        bValueChanged |= function("##W", &value[3], step, min, max);
        applyPropertyChange(oldValue[3], &value[3]);
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::EndTable();
    }

    ImGui::PopID();
    return bValueChanged;
}

bool combo_std_vector_pred(void* data, int idx, const char** out_text)
{
    std::vector<std::string>* vector = reinterpret_cast<std::vector<std::string>*>(data);
    *out_text = vector->at(idx).c_str();
    return true;
}

bool combo_std_list_pred(void* data, int idx, const char** out_text)
{
    std::list<std::string>* vector = reinterpret_cast<std::list<std::string>*>(data);
    *out_text = std::next(vector->begin(), idx)->c_str();
    return true;
}

void ImGui::GText(const char* label, const char* fmt, ...)
{
    PushID(label);

    if (BeginTable("data_table", 2, ImGuiTableFlags_SizingFixedFit))
    {
        TableSetupColumn("Parameter", 0, GetContentRegionAvail().x * widthScale);
        TableSetupColumn("Field", 0, GetContentRegionAvail().x * (1.f - widthScale));

        TableNextColumn();
        Text(label);

        TableNextColumn();

        va_list args;
        va_start(args, fmt);
        TextV(fmt, args);
        va_end(args);

        EndTable();
    }

    PopID();
}

bool ImGui::GTextInput(const char* label, std::string* value, bool enabled)
{
    return SimpleValueEdit(label, value,
        [](const char* label, std::string* value)
        {
            return InputText(label, value);
        });
}

bool ImGui::GCheckbox(const char* label, bool* value)
{
    return SimpleValueEdit(label, value,
        [](const char* label, bool* value)
        {
            return Checkbox(label, value);
        });
}

bool ImGui::GColorEdit3(const char* label, glm::vec3& value)
{
    static glm::vec3 oldValue{ 0.f };

    ImGuiColorEditFlags misc_flags{};
    bool bValueChanged{ false };

    PushID(label);

    if (BeginTable("data_table", 2, ImGuiTableFlags_SizingFixedFit))
    {
        TableSetupColumn("Parameter", 0, GetContentRegionAvail().x * widthScale);
        TableSetupColumn("Field", 0, GetContentRegionAvail().x * (1.f - widthScale));

        TableNextColumn();
        Text(label);

        TableNextColumn();
        bValueChanged = ColorEdit3("##light_color", glm::value_ptr(value));
        applyPropertyChange(oldValue, &value);

        EndTable();
    }

    PopID();

    return bValueChanged;
}

bool ImGui::GColorEdit4(const char* label, glm::vec4& value)
{
    static glm::vec4 oldValue{0.f};

    ImGuiColorEditFlags misc_flags{};
    bool bValueChanged{ false };

    PushID(label);

    if (BeginTable("data_table", 2, ImGuiTableFlags_SizingFixedFit))
    {
        TableSetupColumn("Parameter", 0, GetContentRegionAvail().x * widthScale);
        TableSetupColumn("Field", 0, GetContentRegionAvail().x * (1.f - widthScale));

        TableNextColumn();
        Text(label);

        TableNextColumn();
        bValueChanged = ColorEdit4("##", glm::value_ptr(value), misc_flags);
        applyPropertyChange(oldValue, &value);
        
        EndTable();
    }

    PopID();

    return bValueChanged;
}


bool ImGui::GDragInt(const char* label, int* value, int step, int min, int max)
{
    return DragScalarExt(label, value, step, min, max,
        [](const char* label, int* value, int step, int min, int max)
        {
            return ImGui::DragInt(label, value, step, min, max);
        });
}


bool ImGui::GDragFloat(const char* label, float* value, float step, float min, float max)
{
    return DragScalarExt(label, value, step, min, max,
        [](const char* label, float* value, float step, float min, float max)
        {
            return ImGui::DragFloat(label, value, step, min, max);
        });
}

bool ImGui::GDragDouble(const char* label, double* value, double step, double min, double max)
{
    return DragScalarExt(label, value, step, min, max,
        [](const char* label, double* value, double step, double min, double max)
        {
            return ImGui::DragDouble(label, value, step, min, max);
        });
}

bool ImGui::GDragIntVec3(const char* label, int value[3], int step, int min, int max)
{
    return DragVector3Ext(label, value, step, min, max,
        [](const char* label, int* value, int step, int min, int max)
        {
            return ImGui::DragInt(label, value, step, min, max);
        });
}

bool ImGui::GDragFloatVec3(const char* label, float value[3], float step, float min, float max)
{
    return DragVector3Ext(label, value, step, min, max,
        [](const char* label, float* value, float step, float min, float max)
        {
            return ImGui::DragFloat(label, value, step, min, max);
        });
}

bool ImGui::GDragDoubleVec3(const char* label, double value[3], double step, double min, double max)
{
    return DragVector3Ext(label, value, step, min, max,
        [](const char* label, double* value, double step, double min, double max)
        {
            return ImGui::DragDouble(label, value, step, min, max);
        });
}

bool ImGui::GDragIntVec4(const char* label, int value[4], int step, int min, int max)
{
    return DragVector4Ext(label, value, step, min, max,
        [](const char* label, int* value, int step, int min, int max)
        {
            return ImGui::DragInt(label, value, step, min, max);
        });
}

bool ImGui::GDragFloatVec4(const char* label, float value[4], float step , float min, float max)
{
    return DragVector4Ext(label, value, step, min, max,
        [](const char* label, float* value, float step, float min, float max)
        {
            return ImGui::DragFloat(label, value, step, min, max);
        });
}

bool ImGui::GDragDoubleVec4(const char* label, double value[4], double step, double min, double max)
{
    return DragVector4Ext(label, value, step, min, max,
        [](const char* label, double* value, double step, double min, double max)
        {
            return ImGui::DragDouble(label, value, step, min, max);
        });
}

bool ImGui::GDragTransform(glm::vec3& pos, glm::quat& rot, glm::vec3& scale, float step, float min, float max)
{
    bool bValueChanged{ false };
    bValueChanged |= GDragFloatVec3("position", glm::value_ptr(pos), step, min, max);
    bValueChanged |= GDragFloatVec4("rotation", glm::value_ptr(rot), step, min, max);
    bValueChanged |= GDragFloatVec3("scale", glm::value_ptr(scale), step, min, max);
    return bValueChanged;
}

bool ImGui::GAssetHolder(const char* label, const std::string& source)
{
    bool result{ false };

    PushID(label);

    if (BeginTable("table", 2))
    {
        TableNextColumn();
        Text(label);

        TableNextColumn();
        BeginDisabled(!source.empty());
        result = Button(source.empty() ? "+" : source.c_str(), ImVec2(-1.f, 0.f));
        EndDisabled();

        EndTable();
    }

    PopID();

    return result;
}

bool ImGui::GCombo(const char* label, int* current_item, const char* const items[], int item_count, int height_in_items)
{
    static int oldValue{ 0 };
    bool result{ false };

    PushID(label);

    if (BeginTable("table", 2))
    {
        TableNextColumn();
        Text(label);

        TableNextColumn(); 
        SetNextItemWidth(GetContentRegionAvail().x);
        result = Combo(label, current_item, items, item_count, height_in_items);
        applyPropertyChange(oldValue, current_item);

        EndTable();
    }

    PopID();

    return result;
}

bool ImGui::GSliderInt(const char* label, int* value, int min, int max)
{
    static int oldValue{ 0 };

    bool bValueChanged{ false };
    ImGuiIO& io = GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    PushID(label);

    if (BeginTable("table", 2))
    {
        TableNextColumn(); Text(label);

        TableNextColumn();
        PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        float lineHeight = boldFont->FontSize + 6.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        PushFont(boldFont);
        if (Button("N", buttonSize))
        {
            bValueChanged = true;
            *value = min;
        }
        PopFont();
        PopStyleColor(3);

        SameLine();

        SetNextItemWidth(GetContentRegionAvail().x);
        bValueChanged |= SliderInt("##N", value, min, max);
        applyPropertyChange(oldValue, value);

        PopStyleVar();

        ImGui::EndTable();
    }

    PopID();

    return bValueChanged;
}


bool ImGui::FileOpen(char* buf, size_t buf_size, const char* title, int filter_num, const char* const* filter_patterns)
{
    const char* filename = tinyfd_openFileDialog(title, "", filter_num, filter_patterns, nullptr, false);
    if (filename)
        strcpy(buf, filename);
    return filename;
}

bool ImGui::FileSave(char* buf, size_t buf_size, const char* title, int filter_num, const char* const* filter_patterns)
{
    const char* filename = tinyfd_saveFileDialog(title, "", filter_num, filter_patterns, nullptr);
    if (filename)
        strcpy(buf, filename);
    return filename;
}