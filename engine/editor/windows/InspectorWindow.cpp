#include "InspectorWindow.h"

#include "Engine.h"

#include <imgui/imgui.h>
#include "editor/CustomControls.h"

#include "ecs/components/components.h"

#include "ecs/helper.hpp"

#include "system/filesystem/filesystem.h"

#include "game/SceneGraph.hpp"

#include "loaders/MeshLoader.h"
#include "loaders/ImageLoader.h"

using namespace engine::editor;
using namespace engine::system;
using namespace engine::loaders;
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
		auto& icon = EGEditor->getIcon<_Ty>();
		
		ImGui::PushID(name.c_str());
		auto pre_header = ImGui::GetCursorPos();
		ImVec2 arter_content; 
		if (ImGui::CollapsingHeader((icon + " " + name).c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap))
		{
			editpred(object);
			arter_content = ImGui::GetCursorPos();
		}
			
		auto& trash = EGEditor->getIcon(icons::trash);
		if constexpr (!std::is_same_v<_Ty, FTransformComponent>)
		{
			auto padding = ImGui::GetStyle().WindowPadding;
			pre_header.x = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(trash.c_str()).x + padding.x / 2.f;
			ImGui::SetCursorPos(pre_header);
			if (ImGui::Button(trash.c_str()))
				registry.remove<_Ty>(entity);
			ImGui::SetCursorPos(arter_content);
		}

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
		auto& icon = EGEditor->getIcon<_Ty>();
		if (ImGui::MenuItem((icon + " " + name).c_str()))
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
		auto& hierarchy = registry.get<FHierarchyComponent>(selected);

		if (ImGui::BeginPopup("add_component_popup"))
		{
			try_add_menu_item<FAudioComponent>("Audio", selected);
			try_add_menu_item<FCameraComponent>("Camera", selected);
			try_add_menu_item<FScriptComponent>("Script", selected);
			try_add_menu_item<FEnvironmentComponent>("Environment", selected);
			try_add_menu_item<FDirectionalLightComponent>("Directional light", selected);
			try_add_menu_item<FSpotLightComponent>("Spot light", selected);
			try_add_menu_item<FPointLightComponent>("Point light", selected);
			try_add_menu_item<FSceneComponent>("Scene", selected);
			ImGui::EndPopup();
		}

		ImGui::Separator();

		ImGui::GTextInput("Name", &hierarchy.name);
		ImGui::Separator();

		try_show_edit<FTransformComponent>("Transform", selected,
			[this](auto* object) { ImGui::GDragTransform(object->position, object->rotation, object->scale); });
		try_show_edit<FAudioComponent>("Audio", selected,
			[this](auto* object) { audioEdit(object); });
		try_show_edit<FCameraComponent>("Camera", selected,
			[this](auto* object) { cameraEdit(object); });
		try_show_edit<FMeshComponent>("Mesh", selected,
			[](auto*) {});
		try_show_edit<FScriptComponent>("Script", selected,
			[this](auto* object) { scriptEdit(object); });
		try_show_edit<FEnvironmentComponent>("Environment", selected,
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
				ImGui::GDragFloatVec3("Target", object->target, 0.01f);
				ImGui::GDragFloat("Intencity", &object->intencity, 0.01f, 0.01f, 50.f);
				ImGui::GDragFloat("Inner angle", &object->innerAngle, 0.01f, 0.01f, object->outerAngle);
				ImGui::GDragFloat("Outer angle", &object->outerAngle, 0.01f, object->innerAngle, 3.14f);
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

		auto& plus = EGEditor->getIcon(icons::plus);
		if (ImGui::Button(plus.c_str(), ImVec2(-1.f, 0.f)))
			ImGui::OpenPopup("add_component_popup");

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				auto source = std::filesystem::path(path);

				if (fs::is_audio_format(source))
				{
					if (!registry.try_get<FAudioComponent>(selected))
					{
						FAudioComponent naudio{};
						naudio.source = fs::from_unicode(source);

						registry.emplace<FAudioComponent>(selected, std::move(naudio));
					}
				}
				else if (fs::is_script_format(source))
				{
					if (!registry.try_get<FScriptComponent>(selected))
					{
						FScriptComponent nscript{};
						nscript.source = fs::from_unicode(source);

						registry.emplace<FScriptComponent>(selected, std::move(nscript));
					}
				}
				else if (fs::is_skybox_format(source))
				{
					std::unique_ptr<FImageCreateInfo> pImageCI;
					CImageLoader::load(source, pImageCI);
					if (!registry.try_get<FEnvironmentComponent>(selected) && pImageCI->isCubemap)
					{
						FEnvironmentComponent nskybox{};
						nskybox.source = fs::from_unicode(source);

						registry.emplace<FEnvironmentComponent>(selected, std::move(nskybox));
					}
				}
				else if (fs::is_mesh_format(source))
				{
					if (!registry.try_get<FSceneComponent>(selected))
					{
						FSceneComponent nscene{};
						nscene.source = fs::from_unicode(source);

						registry.emplace<FSceneComponent>(selected, std::move(nscene));
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
}

// audio editor
void CEditorInspector::audioEdit(FAudioComponent* object)
{
	ImGui::GAssetHolder("Source", fs::get_filename(object->source));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			auto source = std::filesystem::path(path);

			if (fs::is_audio_format(source))
			{
				if (object->source != source)
				{
					object->source = fs::from_unicode(source);

					if (object->loaded)
					{
						EGAudio->removeSource(object->asource);
						auto pAudio = std::make_unique<CAudioSource>(object->source);
						object->asource = EGAudio->addSource(fs::get_filename(source), std::move(pAudio));
					}
					else
					{
						auto self = EGEditor->getLastSelection();
						auto& registry = EGCoordinator;

						FAudioComponent naudio{};
						naudio.source = fs::from_unicode(source);

						registry.remove<FAudioComponent>(self);
						registry.emplace<FAudioComponent>(self, std::move(naudio));
					}
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

		auto& play = EGEditor->getIcon(icons::play);
		if (ImGui::Button(play.c_str()))
			pAudio->play();

		ImGui::SameLine();

		auto& pause = EGEditor->getIcon(icons::pause);
		if (ImGui::Button(pause.c_str()))
			pAudio->pause();

		ImGui::SameLine();

		auto& stop = EGEditor->getIcon(icons::stop);
		if (ImGui::Button(stop.c_str()))
			pAudio->stop();

		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		auto pos = pAudio->getPosInSec();
		auto len = pAudio->getLenInSec();

		// TODO: format time to mis:sec
		uint32_t mins = (uint32_t)pos / 60u;
		uint32_t secs = (uint32_t)pos % 60u;
		auto formatted = std::format("{:02}:{:02}", mins, secs);
		if (ImGui::SliderFloat("##player", &pos, 0.f, len, formatted.c_str()))
			pAudio->setOffsetSec(pos);
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
	ImGui::Separator();

	if(ImGui::CollapsingHeader("Post processing", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Chromatic aberration
		FPostProcessCA aberration;
		// Vignette
		FPostProcessVignette vignette;
		// Tonemapping
		FPostProcessTonemap tonemap;
		if (ImGui::CollapsingHeader("FXAA", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::GCheckbox("enable", &object->effects.fxaa.enable);
			ImGui::GDragFloat("quality", &object->effects.fxaa.quality, 0.01f, 0.01f, 1.f);
		}

		if (ImGui::CollapsingHeader("Depth of field", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::GCheckbox("enable", &object->effects.dof.enable);
			//ImGui::GDragInt("bokeh size", &peffects.dof_bokeh_size, 1, 1, 16);
			//ImGui::GDragFloat("bokeh separation", &peffects.dof_bokeh_separation, 0.01f, 0.01f, 5.f);
			//ImGui::GDragFloat("bokeh min", &peffects.dof_bokeh_min_threshold, 0.01f, 0.01f, peffects.dof_bokeh_max_threshold);
			//ImGui::GDragFloat("bokeh max", &peffects.dof_bokeh_max_threshold, 0.01f, peffects.dof_bokeh_min_threshold, 1.f);
			//ImGui::GDragFloat("blur scale", &object->effects.dof.blur_scale, 0.01f, 0.01f, 10.f);
			//ImGui::GDragFloat("blur strength", &object->effects.dof.blur_strength, 0.01f, 0.01f, 10.f);
			ImGui::GDragInt("bokeh_samples", &object->effects.dof.bokeh_samples, 1, 2, 20);
			ImGui::GDragFloat("bokeh_poly", &object->effects.dof.bokeh_poly, 1.f, 1.f, 20.f);
			ImGui::GDragFloat("focus point", &object->effects.dof.focus_point, 0.01f, 0.01f, 100.f);
			ImGui::GDragFloat("near field", &object->effects.dof.near_field, 0.01f, 0.01f, object->effects.dof.far_field);
			ImGui::GDragFloat("near transition", &object->effects.dof.near_transition, 0.01f, 0.01f, 1.f);
			ImGui::GDragFloat("far field", &object->effects.dof.far_field, 0.01f, object->effects.dof.near_field, 100.f);
			ImGui::GDragFloat("far transition", &object->effects.dof.far_transition, 0.01f, 0.01f, 100.f);
		}

		if (ImGui::CollapsingHeader("Bloom", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::GCheckbox("enable", &object->effects.bloom.enable);
			ImGui::GDragFloat("bloom_threshold", &object->effects.bloom.threshold, 0.01f, 0.01f, 1.f);
			ImGui::GDragFloat("radius", &object->effects.bloom.filter_radius, 0.001f, 0.001f, 0.01f);
			ImGui::GDragFloat("strength", &object->effects.bloom.strength, 0.01f, 0.01f, 0.2f);
		}

		if (ImGui::CollapsingHeader("Chromatic aberration", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::GCheckbox("enable", &object->effects.aberration.enable);
			ImGui::GDragFloat("distortion", &object->effects.aberration.distortion, 0.01f, 0.01f, 10.f);
			ImGui::GDragInt("itteration", &object->effects.aberration.iterations, 3, 3, 24);
		}

		if (ImGui::CollapsingHeader("Vignette", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::GCheckbox("enable", &object->effects.vignette.enable);
			ImGui::GDragFloat("inner", &object->effects.vignette.inner, 0.01f, 0.01f, object->effects.vignette.outer);
			ImGui::GDragFloat("outer", &object->effects.vignette.outer, 0.01f, object->effects.vignette.inner, 10.f);
			ImGui::GDragFloat("opacity", &object->effects.vignette.opacity, 0.01f, 0.01f, 1.f);
		}

		if (ImGui::CollapsingHeader("Tonemapping", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::GCheckbox("enable", &object->effects.tonemap.enable);
			ImGui::GDragFloat("gamma", &object->effects.tonemap.gamma, 0.01f, 0.01f, 4.f);
			ImGui::GDragFloat("exposure", &object->effects.tonemap.exposure, 0.01f, 0.01f, 11.f);
		}
	}
}

// script editor
void CEditorInspector::scriptEdit(FScriptComponent* object)
{
	ImGui::GAssetHolder("Script", fs::get_filename(object->source));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			auto source = std::filesystem::path(path);

			if (fs::is_script_format(source))
			{
				object->source = fs::from_unicode(source);
				if (object->loaded)
				{
					EGScripting->removeSource(object->data);
					EGScripting->addSource(object->source, object->source);
				}
				else
				{
					auto& registry = EGCoordinator;
					auto self = EGEditor->getLastSelection();

					FScriptComponent nscript{};
					nscript.source = object->source;

					registry.remove<FScriptComponent>(self);
					registry.emplace<FScriptComponent>(self, std::move(nscript));
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void CEditorInspector::skyboxEdit(FEnvironmentComponent* object)
{
	auto& registry = EGCoordinator;
	auto self = EGEditor->getLastSelection();

	ImGui::GAssetHolder("Source", fs::get_filename(object->source));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			auto source = std::filesystem::path(path);

			if (fs::is_skybox_format(source))
			{
				std::unique_ptr<FImageCreateInfo> pImageCI;
				CImageLoader::load(source, pImageCI);

				if (object->source != source && pImageCI->isCubemap)
				{
					object->source = fs::from_unicode(source);
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
					{
						FEnvironmentComponent nskybox{};
						nskybox.source = object->source;

						registry.remove<FEnvironmentComponent>(self);
						registry.emplace<FEnvironmentComponent>(self, std::move(nskybox));
					}
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
	if(ImGui::GCheckbox("Active", &object->active) && object->active)
		set_active_skybox(registry, self);
}

void CEditorInspector::sceneEdit(FSceneComponent* object)
{
	auto& registry = EGCoordinator;
	auto self = EGEditor->getLastSelection();

	ImGui::GAssetHolder("Source", fs::get_filename(object->source));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			auto source = std::filesystem::path(path);

			if (fs::is_mesh_format(source))
			{
				if (object->source != source)
				{
					object->source = fs::from_unicode(source);
					if (object->loaded)
					{
						auto& hierarchy = registry.get<FHierarchyComponent>(self);
						while (!hierarchy.children.empty())
						{
							auto child = hierarchy.children.front();
							scenegraph::destroy_node(child);
						}

						CMeshLoader::load(object->source, self);
					}
					else
					{
						auto& registry = EGCoordinator;

						FSceneComponent nscene{};
						nscene.source = fs::from_unicode(source);

						registry.remove<FSceneComponent>(self);
						registry.emplace<FSceneComponent>(self, std::move(nscene));
					}
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::Text("");
}