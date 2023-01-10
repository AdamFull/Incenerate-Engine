#include "CustomControls.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>


constexpr const float _columnWidth{100.f};

namespace ImGui
{
    bool GCheckbox(const std::string& label, bool* value)
    {
        bool bValueChanged{ false };

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, _columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        if (*value)
        {
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        }

        bValueChanged = ImGui::Checkbox(("##" + label).c_str(), value);

        ImGui::PopStyleColor(3);

        ImGui::Columns(1);
        ImGui::PopID();

        return bValueChanged;
    }

    bool GColorEdit3(const std::string& label, glm::vec3& value)
    {
        ImGuiColorEditFlags misc_flags{};
        bool bValueChanged{ false };

        PushID(label.c_str());

        Columns(2);
        SetColumnWidth(0, _columnWidth);
        Text(label.c_str());
        NextColumn();

        bValueChanged = ColorEdit3(("##" + label).c_str(), glm::value_ptr(value), misc_flags);

        Columns(1);
        PopID();

        return bValueChanged;
    }

    bool GColorEdit4(const std::string& label, glm::vec4& value)
    {
        ImGuiColorEditFlags misc_flags{};
        bool bValueChanged{ false };

        PushID(label.c_str());

        Columns(2);
        SetColumnWidth(0, _columnWidth);
        Text(label.c_str());
        NextColumn();

        bValueChanged = ColorEdit4(("##" + label).c_str(), glm::value_ptr(value), misc_flags);

        Columns(1);
        PopID();

        return bValueChanged;
    }

    bool GDragFloatVec3(const std::string& label, glm::vec3& values, float step, float min, float max)
    {
        bool bValueChanged{ false };
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        PushID(label.c_str());

        Columns(2);
        SetColumnWidth(0, _columnWidth);
        Text(label.c_str());
        NextColumn();

        PushMultiItemsWidths(3, ImGui::CalcItemWidth());
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
        Columns(1);
        PopID();
        return bValueChanged;
    }

    bool GDragFloat(const std::string& label, float* value, float step, float min, float max)
    {
        bool bValueChanged{ false };
        ImGuiIO& io = GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        PushID(label.c_str());

        Columns(2);
        SetColumnWidth(0, _columnWidth);
        Text(label.c_str());
        NextColumn();

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
        bValueChanged |= DragFloat("##N", value, step, min, max, "%.2f");
        //ImGui::PopItemWidth();

        PopStyleVar();
        Columns(1);
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
}