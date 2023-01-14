#include "CustomControls.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>


constexpr const float _columnWidth{100.f};

namespace ImGui
{
    bool GCheckbox(const std::string& label, bool* value)
    {
        bool bValueChanged{ false };

        ImGui::PushID(label.c_str());

        if (BeginTable(("table" + label).c_str(), 2))
        {
            TableNextColumn(); ImGui::Text(label.c_str());
            TableNextColumn();

            bValueChanged = ImGui::Checkbox(("##" + label).c_str(), value);

            EndTable();
        }

        ImGui::PopID();

        return bValueChanged;
    }

    bool GColorEdit3(const std::string& label, glm::vec3& value)
    {
        ImGuiColorEditFlags misc_flags{};
        bool bValueChanged{ false };

        PushID(label.c_str());

        if (BeginTable(("table" + label).c_str(), 2))
        {
            TableNextColumn(); Text(label.c_str());

            TableNextColumn();

            SetNextItemWidth(GetContentRegionAvail().x);
            bValueChanged = ColorEdit3(("##" + label).c_str(), glm::value_ptr(value), misc_flags);

            EndTable();
        }

        PopID();

        return bValueChanged;
    }

    bool GColorEdit4(const std::string& label, glm::vec4& value)
    {
        ImGuiColorEditFlags misc_flags{};
        bool bValueChanged{ false };

        PushID(label.c_str());

        if (BeginTable(("table" + label).c_str(), 2))
        {
            TableNextColumn(); Text(label.c_str());

            TableNextColumn();

            SetNextItemWidth(GetContentRegionAvail().x);
            bValueChanged = ColorEdit4(("##" + label).c_str(), glm::value_ptr(value), misc_flags);
            
            EndTable();
        }

        PopID();

        return bValueChanged;
    }

    bool GDragFloatVec3(const std::string& label, glm::vec3& values, float step, float min, float max)
    {
        bool bValueChanged{ false };
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        PushID(label.c_str());


        if (BeginTable(("table" + label).c_str(), 2))
        {
            TableNextColumn(); Text(label.c_str());

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
            PopItemWidth();

            PopStyleVar();

            ImGui::EndTable();
        }

        PopID();
        return bValueChanged;
    }

    bool GDragFloat(const std::string& label, float* value, float step, float min, float max)
    {
        bool bValueChanged{ false };
        ImGuiIO& io = GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        PushID(label.c_str());

        if (BeginTable(("table" + label).c_str(), 2))
        {
            TableNextColumn(); Text(label.c_str());

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
                *value = 0.0f;
            }
            PopFont();
            PopStyleColor(3);

            SameLine();

            SetNextItemWidth(GetContentRegionAvail().x);
            bValueChanged |= DragFloat("##N", value, step, min, max, "%.2f");

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

    bool GAssetHolder(const std::string& label, const std::string& source)
    {
        auto avaliable_width = GetContentRegionAvail().x;
        bool result{ false };

        PushID(label.c_str());

        if (BeginTable(("table" + label).c_str(), 2))
        {
            TableNextColumn();
            Text(label.c_str());

            TableNextColumn();
            BeginDisabled(!source.empty());
            result = Button(source.empty() ? "+" : source.c_str(), ImVec2(-1.f, 0.f));
            EndDisabled();

            EndTable();
        }

        PopID();

        return result;
    }

    bool GTextInput(const std::string& label, std::string* source, bool enabled)
    {
        auto avaliable_width = GetContentRegionAvail().x;
        bool result{ false };

        PushID(label.c_str());

        if (BeginTable(("table" + label).c_str(), 2))
        {
            TableNextColumn(); 
            Text(label.c_str());

            TableNextColumn(); 
            SetNextItemWidth(GetContentRegionAvail().x);
            BeginDisabled(!enabled);
            result = InputText(("##" + label).c_str(), source);
            EndDisabled();

            EndTable();
        }

        PopID();

        return result;
    }
}