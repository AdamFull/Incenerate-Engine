#include "InspectorWindow.h"

#include "Engine.h"

#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <imgui/IconsFontAwesome6.h>
#include "editor/CustomControls.h"

#include "ecs/components/TransformComponent.h"
#include "ecs/components/HierarchyComponent.h"
#include "ecs/components/AudioComponent.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/components/MeshComponent.h"
#include "ecs/components/ScriptComponent.h"
#include "ecs/components/SkyboxComponent.h"
#include "ecs/components/DirectionalLightComponent.h"
#include "ecs/components/PointLightComponent.h"
#include "ecs/components/SpotLightComponent.h"

using namespace engine::editor;
using namespace engine::ecs;

template<class _Ty, class _Pred>
void try_show_edit(const std::string& name, const entt::entity& entity, _Pred&& predicate)
{
	using namespace engine;
	auto& registry = EGCoordinator;
	if (auto object = registry.try_get<_Ty>(entity))
	{
		ImGui::PushID(name.c_str());
		ImGui::Text(name.c_str());

		ImGui::SameLine(ImGui::GetWindowWidth() - 30);
		if (ImGui::Button("X##remove"))
			registry.remove<_Ty>(entity);

		predicate(object);

		ImGui::PopID();
		ImGui::Separator();
	}
}

template<class _Ty>
void try_add_menu_item(const std::string& name, const entt::entity& entity)
{
	using namespace engine;
	auto& registry = EGCoordinator;
	if (!registry.try_get<_Ty>(entity))
	{
		if (ImGui::MenuItem(name.c_str()))
			registry.emplace<_Ty>(entity, _Ty{});
	}
}

void CEditorInspector::create()
{

}

void CEditorInspector::__draw()
{
	auto& registry = EGCoordinator;
	auto& selected = EGEditor->getLastSelection();

	if (selected != entt::null)
	{
		auto& transform = registry.get<FTransformComponent>(selected);
		auto& hierarchy = registry.get<FHierarchyComponent>(selected);

		if (ImGui::BeginPopup("add_component_popup"))
		{
			try_add_menu_item<FAudioComponent>("Audio", selected);
			try_add_menu_item<FCameraComponent>("Camera", selected);
			try_add_menu_item<FScriptComponent>("Script", selected);
			try_add_menu_item<FSkyboxComponent>("Skybox", selected);
			try_add_menu_item<FDirectionalLightComponent>("Directional light", selected);
			try_add_menu_item<FSpotLightComponent>("Spot light", selected);
			try_add_menu_item<FPointLightComponent>("Point light", selected);
			ImGui::EndPopup();
		}

		ImGui::Separator();

		ImGui::InputText("Name", &hierarchy.name);
		ImGui::Separator();

		ImGui::Text("Transform component");
		ImGui::GDragTransform(transform.position, transform.rotation, transform.scale);
		ImGui::Separator();

		try_show_edit<FAudioComponent>("Audio", selected, 
			[](auto* object)
			{
				ImGui::InputText("Source", &object->source);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						auto source = std::filesystem::path(path);
						auto ext = source.extension();

						if (ext == ".wav" || ext == ".ogg")
						{
							// Update skybox here
							object->source = source.string();
						}
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::GDragFloat("Gain", &object->gain, 0.01f, 0.01f, 10.f);
				ImGui::GDragFloat("Pitch", &object->pitch, 0.01f, 0.01f, 10.f);
				ImGui::GCheckbox("Loop", &object->loop);
			});

		try_show_edit<FCameraComponent>("Camera", selected, 
			[](auto* object)
			{
				ImGui::GDragFloat("FoV", &object->fieldOfView, 0.01f, 0.01f, 180.f);
				ImGui::GDragFloat("Near", &object->nearPlane, 0.01f, 0.01f, 1024.f);
				ImGui::GDragFloat("Far", &object->farPlane, 0.01f, 0.01f, 2048.f);
				ImGui::GDragFloat("Sensitivity", &object->sensitivity, 0.01f, 1.f, 50.f);
				ImGui::GCheckbox("Active", &object->active);
			});

		try_show_edit<FMeshComponent>("Mesh", selected,
			[](auto* object)
			{
			});

		try_show_edit<FScriptComponent>("Script", selected,
			[](auto* object)
			{
				ImGui::InputText("Script", &object->source);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						auto source = std::filesystem::path(path);
						auto ext = source.extension();

						if (ext == ".lua")
						{
							// Update skybox here
							object->source = source.string();
						}
					}
					ImGui::EndDragDropTarget();
				}
			});

		try_show_edit<FSkyboxComponent>("Skybox", selected,
			[](auto* object)
			{
				ImGui::InputText("Source", &object->source);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						auto source = std::filesystem::path(path);
						auto ext = source.extension();

						if (ext == ".ktx" || ext == ".ktx2")
						{
							// Update skybox here
							object->source = source.string();
						}
					}
					ImGui::EndDragDropTarget();
				}
			});

		try_show_edit<FDirectionalLightComponent>("Directional light", selected,
			[](auto* object)
			{
				ImGui::GColorEdit3("Color", object->color);
				ImGui::GDragFloat("Intencity", &object->intencity, 0.01f, 0.01f, 50.f);
				ImGui::GCheckbox("Cast shadows", &object->castShadows);
			});

		try_show_edit<FSpotLightComponent>("Spot light", selected,
			[](auto* object)
			{
				ImGui::GColorEdit3("Color", object->color);
				ImGui::GDragFloat("Intencity", &object->intencity, 0.01f, 0.01f, 50.f);
				ImGui::GDragFloat("Inner angle", &object->innerAngle, 0.01f, 0.01f);
				ImGui::GDragFloat("Outer angle", &object->outerAngle, 0.01f, 0.01f);
				ImGui::GCheckbox("To target", &object->toTarget);
				ImGui::GCheckbox("Cast shadows", &object->castShadows);
			});

		try_show_edit<FPointLightComponent>("Point light", selected,
			[](auto* object)
			{
				ImGui::GColorEdit3("Color", object->color);
				ImGui::GDragFloat("Intencity", &object->intencity, 0.01f, 0.01f, 50.f);
				ImGui::GDragFloat("Radius", &object->radius, 0.01f, 0.01f);
				ImGui::GCheckbox("Cast shadows", &object->castShadows);
			});

		if (ImGui::Button("+##add_component", ImVec2(-1.f, 0.f)))
			ImGui::OpenPopup("add_component_popup");
	}
}