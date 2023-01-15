#include "SystemPerfomanceViewWindow.h"

#include <imgui/imgui.h>

#include "Engine.h"

using namespace engine::editor;

void CEditorPerfomanceView::create()
{
	bIsOpen = false;
}

void CEditorPerfomanceView::__draw(float fDt)
{
	auto& systems = EGEngine->getSystems();

    ImGui::Text("Create time");
    if (ImGui::BeginTable("create_time_table" , 2))
    {
        for (auto& system : systems)
        {
            auto& name = system->getName();
            auto ct = system->createElapsed();

            ImGui::TableNextColumn();
            ImGui::Text(name.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%.10fs", ct);
        }

        ImGui::EndTable();
    }

    ImGui::Separator();

    ImGui::Text("Update time");
    if (ImGui::BeginTable("update_time_table", 2))
    {
        for (auto& system : systems)
        {
            auto& name = system->getName();
            auto ut = system->updateElapsed();

            ImGui::TableNextColumn();
            ImGui::Text(name.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%.10fs", ut);
        }

        ImGui::EndTable();
    }
}