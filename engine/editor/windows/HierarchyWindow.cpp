#include "HierarchyWindow.h"

#include "Engine.h"

#include "ecs/components/HierarchyComponent.h"

#include <imgui/imgui.h>

#include "game/SceneGraph.hpp"

#include "editor/operations/CreateEntityOperation.h"
#include "editor/operations/RemoveEntityOperation.h"
#include "editor/operations/CopyEntityOperation.h"
#include "editor/operations/DuplicateEntityOperation.h"

using namespace engine::editor;
using namespace engine::game;
using namespace engine::ecs;

void CEditorHierarchy::create()
{

}

void CEditorHierarchy::__draw(float fDt)
{
    auto& actionBuffer = EGEditor->getActionBuffer();
    auto& registry = EGCoordinator;
    auto root = EGSceneManager->getRoot();
	auto current_size = ImGui::GetWindowSize();

	buildHierarchy(root);

	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
	{
		if (ImGui::IsMouseClicked(1))
			ImGui::OpenPopup("HierarchyOptions");
	}

    if (ImGui::BeginPopup("HierarchyOptions"))
    {
        if (ImGui::MenuItem("create"))
            actionBuffer->addOperation(std::make_unique<CCreateEntityOperation>(selected_entity));

        if (ImGui::MenuItem("copy", "CTRL+C"))
            copy_entity = selected_entity;

        if (ImGui::MenuItem("paste", "CTRL+V", nullptr, copy_entity != entt::null))
        {
            actionBuffer->addOperation(std::make_unique<CCopyEntityOperation>(selected_entity, copy_entity));
            copy_entity = entt::null;
        }

        if (ImGui::MenuItem("duplicate"))
        {
            entt::entity attach_to{ root };
            auto& hierarchy = registry.get<FHierarchyComponent>(selected_entity);

            if (hierarchy.parent != entt::null)
                attach_to = hierarchy.parent;

            actionBuffer->addOperation(std::make_unique<CDuplicateEntityOperation>(attach_to, selected_entity));
        }

        if (ImGui::MenuItem("delete", "DEL"))
        {
            if (EGEditor->isSelected(selected_entity))
                EGEditor->selectObject(root);

            actionBuffer->addOperation(std::make_unique<CRemoveEntityOperation>(selected_entity));
        }

        ImGui::EndPopup();
    }

    int size_x = current_size.x;
    int size_y = current_size.y - ImGui::GetCursorPosY() - 10;
    ImGui::SetCursorPosX(0);
    if (ImGui::InvisibleButton("###unselect_all_btn", ImVec2(size_x, size_y)))
    {
        selected_entity = root;
        EGEditor->deselectAll();
    }
}

void CEditorHierarchy::buildHierarchy(const entt::entity& entity)
{
    if (entity != entt::null)
    {
        std::string icon{};

        uint32_t flags = ImGuiTreeNodeFlags_OpenOnArrow;
        auto& registry = EGCoordinator;

        auto& hierarchy = registry.get<FHierarchyComponent>(entity);

        //Is object selected
        auto isSelected = EGEditor->isSelected(entity);
        if (isSelected)
            flags |= ImGuiTreeNodeFlags_Selected;

        //Has object childs
        if (hierarchy.children.empty())
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        else
            icon = EGEditor->getIcon(icons::node);

        ImGui::PushID(static_cast<uint32_t>(entity));
        bool isOpen = ImGui::TreeNodeEx((icon + " " + hierarchy.name).c_str(), flags);
        ImGui::PopID();

        if(ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))
            selected_entity = entity;

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

        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("scene_node", &entity, sizeof(entity));
            ImGui::Text(hierarchy.name.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            auto payload = ImGui::AcceptDragDropPayload("scene_node");
            if (payload)
            {
                auto node = *static_cast<entt::entity*>(payload->Data);
                scenegraph::parent_exchange(entity, node);
            }
            
            ImGui::EndDragDropTarget();
        }

        if (isOpen && !hierarchy.children.empty())
        {
            for (auto& child : hierarchy.children)
                buildHierarchy(child);
            ImGui::TreePop();
        }
    }
}