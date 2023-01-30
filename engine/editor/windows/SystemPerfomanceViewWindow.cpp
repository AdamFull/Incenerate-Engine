#include "SystemPerfomanceViewWindow.h"

#include <imgui/imgui.h>

#include "Engine.h"
#include "ecs/systems/rendering/BaseGraphicsSystem.h"

using namespace engine::ecs;
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
    for (auto& system : systems)
    {
        expand(system.get());
    }
}

void CEditorPerfomanceView::expand(ISystem* system)
{
    auto& name = system->getName();
    auto ut = system->updateElapsed();

    ImGui::PushID(name.c_str());

    if (ImGui::BeginTable("table", 2))
    {
        ImGui::TableNextColumn();
        ImGui::Text(name.c_str());
        ImGui::TableNextColumn();
        ImGui::Text("%.10fs", ut);

        ImGui::EndTable();
    }

    if (auto sys = dynamic_cast<CBaseGraphicsSystem*>(system))
    {
        for (auto& subsys : sys->getSubSystems())
            expand(subsys.get());
    }

    ImGui::PopID();
}