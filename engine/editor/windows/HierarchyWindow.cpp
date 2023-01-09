#include "HierarchyWindow.h"

#include "Engine.h"

#include "ecs/components/HierarchyComponent.h"

#include <imgui/imgui.h>

using namespace engine::editor;
using namespace engine::ecs;

void CEditorHierarchy::create()
{

}

void CEditorHierarchy::draw()
{
	if (bIsOpen)
	{
		if (ImGui::Begin("Hierarchy", &bIsOpen))
		{
			auto current_size = ImGui::GetWindowSize();

			buildHierarchy(EGSceneGraph);

			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
			{
				if (ImGui::IsMouseClicked(1))
					ImGui::OpenPopup("HierarchyOptions");
			}

            if (ImGui::BeginPopup("HierarchyOptions"))
            {
                if (ImGui::MenuItem("copy"));
                if (ImGui::MenuItem("paste"));
                if (ImGui::MenuItem("duplicate"));
                if (ImGui::MenuItem("delete"));

                ImGui::Separator();

                if (ImGui::BeginMenu("create"))
                {
                    if (ImGui::BeginMenu("primitive"))
                    {
                        if (ImGui::MenuItem("plane"));
                        if (ImGui::MenuItem("cube"));
                        if (ImGui::MenuItem("sphere"));
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("light"))
                    {
                        if (ImGui::MenuItem("point light"));
                        if (ImGui::MenuItem("spot light"));
                        if (ImGui::MenuItem("directional light"));
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("mesh"))
                    {
                        if (ImGui::MenuItem("static mesh"));
                        if (ImGui::MenuItem("skeletal mesh"));
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndPopup();
            }

            int size_x = current_size.x;
            int size_y = current_size.y - ImGui::GetCursorPosY() - 10;
            ImGui::SetCursorPosX(0);
            if (ImGui::InvisibleButton("###unselect_all_btn", ImVec2(size_x, size_y)))
            {
                EGEditor->deselectAll();
            }

			ImGui::End();
		}
	}
}

void CEditorHierarchy::buildHierarchy(const entt::entity& entity)
{
    if (entity != entt::null)
    {
        uint32_t flags = ImGuiTreeNodeFlags_OpenOnArrow;
        auto& registry = EGCoordinator;

        auto& hierarchy = registry.get<FHierarchyComponent>(entity);

        //Is object selected
        auto isSelected = EGEditor->isSelected(entity);
        if (isSelected)
            flags |= ImGuiTreeNodeFlags_Selected;

        //Has object childs
        if (!hierarchy.children.empty())
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        ImGui::PushID(static_cast<uint32_t>(entity));
        bool isOpen = ImGui::TreeNodeEx(hierarchy.name.c_str(), flags);

        //Mouse double click event
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            //TODO: Add object lookAt
        }

        if (ImGui::IsItemHovered())
        {
            // Ctrl + click
            if (ImGui::IsKeyPressed(ImGuiKey_LeftCtrl) && ImGui::IsItemClicked(0))
            {
                if (isSelected)
                    EGEditor->deselectObject(entity);
                else
                    EGEditor->selectObject(entity);
            }
            //Mouse click event
            else if (!ImGui::IsKeyPressed(ImGuiKey_LeftCtrl) && ImGui::IsItemClicked(0))
            {
                EGEditor->deselectAll();
                EGEditor->selectObject(entity);
            }
        }

        if (isOpen && !hierarchy.children.empty())
        {
            for (auto& child : hierarchy.children)
                buildHierarchy(child);
            ImGui::TreePop();
        }
    }
}