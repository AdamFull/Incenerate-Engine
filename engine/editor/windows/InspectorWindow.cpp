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
#include "ecs/components/SceneComponent.h"

using namespace engine::editor;
using namespace engine::game;
using namespace engine::audio;
using namespace engine::ecs;

template<class _Ty, class _EditPred>
void try_show_edit(const std::string& name, const entt::entity& entity, _EditPred&& editpred)
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

		editpred(object);

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

void CEditorInspector::__draw(float fDt)
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
			try_add_menu_item<FSceneComponent>("Scene", selected);
			ImGui::EndPopup();
		}

		ImGui::Separator();

		ImGui::InputText("Name", &hierarchy.name);
		ImGui::Separator();

		ImGui::Text("Transform component");
		ImGui::GDragTransform(transform.position, transform.rotation, transform.scale);
		ImGui::Separator();

		try_show_edit<FAudioComponent>("Audio", selected, 
			[this](auto* object) { audioEdit(object); });
		try_show_edit<FCameraComponent>("Camera", selected, 
			[this](auto* object) { cameraEdit(object); });
		try_show_edit<FMeshComponent>("Mesh", selected, 
			[](auto*) {});
		try_show_edit<FScriptComponent>("Script", selected, 
			[this](auto* object) { scriptEdit(object); });
		try_show_edit<FSkyboxComponent>("Skybox", selected, 
			[this](auto* object) { skyboxEdit(object); });
		try_show_edit<FSceneComponent>("Scene", selected, 
			[this](auto* object) { sceneEdit(object); });

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

// audio editor
void CEditorInspector::audioEdit(FAudioComponent* object)
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
				if (object->source != source)
				{
					object->source = source.string();

					if (object->loaded)
					{
						EGAudio->removeSource(object->asource);
						auto pAudio = std::make_unique<CAudioSource>(object->source);
						object->asource = EGAudio->addSource(source.filename().string(), std::move(pAudio));
					}
					else
						object->create();
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::GDragFloat("Gain", &object->gain, 0.01f, 0.01f, 10.f);
	ImGui::GDragFloat("Pitch", &object->pitch, 0.01f, 0.01f, 10.f);
	ImGui::GCheckbox("Loop", &object->loop);

	auto& pAudio = EGAudio->getSource(object->asource);
	if (pAudio)
	{
		ImGui::Text("Player");

		auto pos = pAudio->getPosInSec();
		auto len = pAudio->getLenInSec();

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		if (ImGui::SliderFloat("##player", &pos, 0.f, len, "%.2f sec"))
			pAudio->setOffsetSec(pos);

		if (ImGui::Button("Play"))
			pAudio->play();

		ImGui::SameLine();

		if (ImGui::Button("Stop"))
			pAudio->stop();

		ImGui::SameLine();

		if (ImGui::Button("Pause"))
			pAudio->pause();
	}
}

// camera editor
void CEditorInspector::cameraEdit(FCameraComponent* object)
{
	ImGui::GDragFloat("FoV", &object->fieldOfView, 0.01f, 0.01f, 180.f);
	ImGui::GDragFloat("Near", &object->nearPlane, 0.01f, 0.01f, 1024.f);
	ImGui::GDragFloat("Far", &object->farPlane, 0.01f, 0.01f, 2048.f);
	ImGui::GDragFloat("Sensitivity", &object->sensitivity, 0.01f, 1.f, 50.f);
	ImGui::GCheckbox("Active", &object->active);
}

// script editor
void CEditorInspector::scriptEdit(FScriptComponent* object)
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
}

void CEditorInspector::skyboxEdit(FSkyboxComponent* object)
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
				if (object->source != source)
				{
					object->source = source.string();
					if (object->loaded)
					{
						EGGraphics->removeImage(object->prefiltred);
						EGGraphics->removeImage(object->irradiance);
						EGGraphics->removeImage(object->origin);
						object->origin = EGGraphics->addImage(object->source, object->source);
						object->irradiance = EGGraphics->computeIrradiance(object->origin, 64);
						object->prefiltred = EGGraphics->computePrefiltered(object->origin, 512);
					}
					else
						object->create();
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void CEditorInspector::sceneEdit(FSceneComponent* object)
{
	auto& registry = EGCoordinator;
	auto self = EGEditor->getLastSelection();

	ImGui::InputText("Source", &object->source);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			auto source = std::filesystem::path(path);
			auto ext = source.extension();

			if (ext == ".gltf" || ext == ".glb")
			{
				if (object->source != source)
				{
					object->source = source.string();
					if (object->loaded)
					{
						auto& hierarchy = registry.get<FHierarchyComponent>(object->self);
						for (auto& child : hierarchy.children)
							scenegraph::destroy_node(child);
						object->create(self);
					}
					else
						object->create(self);
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
}