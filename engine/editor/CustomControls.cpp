#include "CustomControls.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <tinyfiledialogs/tinyfiledialogs.h>

#include "Engine.h"
#include "editor/operations/PropertyChangedOperation.h"

using namespace engine;
using namespace engine::editor;

constexpr const float _columnWidth{100.f};

namespace ImGui
{
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

    bool GCheckbox(const char* label, bool* value)
    {
        static bool oldValue{ false };
        bool bValueChanged{ false };

        ImGui::PushID(label);

        if (BeginTable("table", 2))
        {
            TableNextColumn(); ImGui::Text(label);
            TableNextColumn();

            bValueChanged = ImGui::Checkbox("##", value);
            applyPropertyChange(oldValue, value);

            EndTable();
        }

        ImGui::PopID();

        return bValueChanged;
    }

    bool GColorEdit3(const char* label, glm::vec3& value)
    {
        static glm::vec3 oldValue{ 0.f };

        ImGuiColorEditFlags misc_flags{};
        bool bValueChanged{ false };

        PushID(label);

        if (BeginTable("table", 2))
        {
            TableNextColumn(); Text(label);

            TableNextColumn();

            SetNextItemWidth(GetContentRegionAvail().x);
            bValueChanged = ColorEdit3("##", glm::value_ptr(value), misc_flags);
            applyPropertyChange(oldValue, &value);

            EndTable();
        }

        PopID();

        return bValueChanged;
    }

    bool GColorEdit4(const char* label, glm::vec4& value)
    {
        static glm::vec4 oldValue{0.f};

        ImGuiColorEditFlags misc_flags{};
        bool bValueChanged{ false };

        PushID(label);

        if (BeginTable("table", 2))
        {
            TableNextColumn(); Text(label);

            TableNextColumn();

            SetNextItemWidth(GetContentRegionAvail().x);
            bValueChanged = ColorEdit4("##", glm::value_ptr(value), misc_flags);
            applyPropertyChange(oldValue, &value);
            
            EndTable();
        }

        PopID();

        return bValueChanged;
    }

    bool GDragFloatVec3(const char* label, glm::vec3& values, float step, float min, float max)
    {
        static glm::vec3 oldValue{ 0.f };

        bool bValueChanged{ false };
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        PushID(label);


        if (BeginTable("table", 2))
        {
            TableNextColumn(); Text(label);

            TableNextColumn();

            PushMultiItemsWidths(3, CalcItemWidth());
            PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

            float lineHeight = boldFont->FontSize + 6.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

            PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            PushFont(boldFont);
            if (Button("X", buttonSize))
            {
                bValueChanged = true;
                values.x = 0.0f;
            }
            PopFont();
            PopStyleColor(3);

            SameLine();
            bValueChanged |= DragFloat("##X", &values.x, step, min, max, "%.2f");
            applyPropertyChange(oldValue.x, &values.x);
            PopItemWidth();
            SameLine();

            PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
            PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            PushFont(boldFont);
            if (ImGui::Button("Y", buttonSize))
            {
                bValueChanged = true;
                values.y = 0.0f;
            }
            PopFont();
            PopStyleColor(3);

            SameLine();
            bValueChanged |= DragFloat("##Y", &values.y, step, min, max, "%.2f");
            applyPropertyChange(oldValue.y, &values.y);
            PopItemWidth();
            SameLine();

            PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
            PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            PushFont(boldFont);
            if (Button("Z", buttonSize))
            {
                bValueChanged = true;
                values.z = 0.0f;
            }

            PopFont();
            PopStyleColor(3);

            SameLine();
            bValueChanged |= DragFloat("##Z", &values.z, step, min, max, "%.2f");
            applyPropertyChange(oldValue.z, &values.z);
            PopItemWidth();

            PopStyleVar();

            ImGui::EndTable();
        }

        PopID();
        return bValueChanged;
    }

    bool GDragFloat(const char* label, float* value, float step, float min, float max)
    {
        static float oldValue{ 0.f };

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
            bValueChanged = DragFloat("##N", value, step, min, max, "%.3f");
            applyPropertyChange(oldValue, value);
                
            PopStyleVar();

            ImGui::EndTable();
        }

        PopID();

        return bValueChanged;
    }

    bool GDragInt(const char* label, int* value, int step, int min, int max)
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
            bValueChanged |= DragInt("##N", value, step, min, max);
            applyPropertyChange(oldValue, value);

            PopStyleVar();

            ImGui::EndTable();
        }

        PopID();

        return bValueChanged;
    }

    bool GDragTransform(glm::vec3& pos, glm::vec3& rot, glm::vec3& scale, float step, float min, float max)
    {
        bool bValueChanged{ false };
        bValueChanged |= GDragFloatVec3("position", pos, step, min, max);
        bValueChanged |= GDragFloatVec3("rotation", rot, step, min, max);
        bValueChanged |= GDragFloatVec3("scale", scale, step, min, max);
        return bValueChanged;
    }

    bool GAssetHolder(const char* label, const std::string& source)
    {
        auto avaliable_width = GetContentRegionAvail().x;
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

    bool GTextInput(const char* label, std::string* source, bool enabled)
    {
        static std::string oldValue;

        bool result{ false };

        PushID(label);

        if (BeginTable("table", 2))
        {
            TableNextColumn(); 
            Text(label);

            TableNextColumn(); 
            SetNextItemWidth(GetContentRegionAvail().x);
            BeginDisabled(!enabled);
            result = InputText("##", source);
            applyPropertyChange(oldValue, source);
            EndDisabled();

            EndTable();
        }

        PopID();

        return result;
    }

    bool GCombo(const char* label, int* current_item, const char* const items[], int item_count, int height_in_items)
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


    bool FileOpen(char* buf, size_t buf_size, const char* title, int filter_num, const char* const* filter_patterns)
    {
        
        const char* filename = tinyfd_openFileDialog(title, "", filter_num, filter_patterns, nullptr, false);
        if (filename)
            strcpy(buf, filename);
        return filename;
    }

    bool FileSave(char* buf, size_t buf_size, const char* title, int filter_num, const char* const* filter_patterns) 
    {
        const char* filename = tinyfd_saveFileDialog(title, "", filter_num, filter_patterns, nullptr);
        if (filename)
            strcpy(buf, filename);
        return filename;
    }
}