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
    for (auto& system : systems)
        expand_create(system.get());

    ImGui::Separator();

    ImGui::Text("Update time");
    for (auto& system : systems)
        expand_update(system.get());
}

void CEditorPerfomanceView::expand_create(ecs::ISystem* system)
{
    uint32_t flags = ImGuiTreeNodeFlags_OpenOnArrow;

    auto sys = dynamic_cast<CBaseGraphicsSystem*>(system);

    if (!sys)
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    else
    {
        if (sys->getSubSystems().empty())
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    auto& name = system->getName();
    auto ut = system->createElapsed();

    bool isOpen = ImGui::TreeNodeEx(name.c_str(), flags, "%s: %.10fs", name.c_str(), ut);

    if (isOpen && sys && !sys->getSubSystems().empty())
    {
        for (auto& subsys : sys->getSubSystems())
            expand_create(subsys.get());
        ImGui::TreePop();
    }
}

void CEditorPerfomanceView::expand_update(ISystem* system)
{
    uint32_t flags = ImGuiTreeNodeFlags_OpenOnArrow;

    auto sys = dynamic_cast<CBaseGraphicsSystem*>(system);

    if (!sys)
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    else
    {
        if(sys->getSubSystems().empty())
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    auto& name = system->getName();
    auto ut = system->updateElapsed();

    bool isOpen = ImGui::TreeNodeEx(name.c_str(), flags, "%s: %.10fs", name.c_str(), ut);

    if (isOpen && sys && !sys->getSubSystems().empty())
    {
        for (auto& subsys : sys->getSubSystems())
            expand_update(subsys.get());
        ImGui::TreePop();
    }
}