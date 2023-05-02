#include "InspectorWindow.h"

#include "Engine.h"

#include <imgui/imgui.h>
#include "editor/CustomControls.h"

#include "ecs/components/components.h"

#include "ecs/helper.hpp"

#include "filesystem/vfs_helper.h"

#include "game/SceneGraph.hpp"

#include "loaders/MeshLoader.h"
#include "graphics/image/ImageLoader.h"

#include "editor/operations/AddComponentOperation.h"
#include "editor/operations/RemoveComponentOperation.h"

using namespace engine::editor;
using namespace engine::filesystem;
using namespace engine::graphics;
using namespace engine::loaders;
using namespace engine::game;
using namespace engine::audio;
using namespace engine::ecs;

template<class _Ty, class _EditPred>
void try_show_edit(const std::string& name, const entt::entity& entity, _EditPred&& editpred)
{
	using namespace engine;

	auto& actionBuffer = EGEditor->getActionBuffer();
	auto& registry = EGEngine->getRegistry();
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
				actionBuffer->addOperation(std::make_unique<CRemoveComponentOperation>(entity, utl::type_hash<_Ty>()));
				
			ImGui::SetCursorPos(arter_content);
		}

		ImGui::PopID();
		ImGui::Separator();
	}
}

template<class _Ty>
void try_add_menu_item_i(const std::string& name, const entt::entity& entity)
{
	using namespace engine;
	auto& actionBuffer = EGEditor->getActionBuffer();
	auto& registry = EGEngine->getRegistry();
	if (!registry.try_get<_Ty>(entity))
	{
		auto& icon = EGEditor->getIcon<_Ty>();
		if (ImGui::MenuItem((icon + " " + name).c_str()))
			actionBuffer->addOperation(std::make_unique<CAddComponentOperation>(entity, utl::type_hash<_Ty>()));
	}
}

void CEditorInspector::create()
{
	
}

void CEditorInspector::__draw(float fDt)
{
	auto& graphics = EGEngine->getGraphics();
	auto& debug_draw = graphics->getDebugDraw();
	auto& registry = EGEngine->getRegistry();
	auto& selected = EGEditor->getLastSelection();

	// TODO: add replace component operation
	if (selected != entt::null)
	{
		auto& transform = registry.get<FTransformComponent>(selected);
		auto& hierarchy = registry.get<FHierarchyComponent>(selected);
		
		if (ImGui::BeginPopup("add_component_popup"))
		{
			try_add_menu_item_i<FAudioComponent>("Audio", selected);
			try_add_menu_item_i<FCameraComponent>("Camera", selected);
			try_add_menu_item_i<FScriptComponent>("Script", selected);
			try_add_menu_item_i<FEnvironmentComponent>("Environment", selected);
			try_add_menu_item_i<FDirectionalLightComponent>("Directional light", selected);
			try_add_menu_item_i<FSpotLightComponent>("Spot light", selected);
			try_add_menu_item_i<FPointLightComponent>("Point light", selected);
			try_add_menu_item_i<FSceneComponent>("Scene", selected);
			try_add_menu_item_i<FRigidBodyComponent>("RigidBody", selected);
			try_add_menu_item_i<FParticleComponent>("Particle system", selected);
			try_add_menu_item_i<FTerrainComponent>("Terrain", selected);
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
			[this](auto* object) { meshEdit(object); });
		try_show_edit<FScriptComponent>("Script", selected,
			[this](auto* object) { scriptEdit(object); });
		try_show_edit<FEnvironmentComponent>("Environment", selected,
			[this](auto* object) { skyboxEdit(object); });
		try_show_edit<FSceneComponent>("Scene", selected,
			[this](auto* object) { sceneEdit(object); });
		try_show_edit<FRigidBodyComponent>("RigidBody", selected,
			[this, &transform](auto* object) { rigidbodyEdit(&transform, object); });
		try_show_edit<FParticleComponent>("Particle system", selected,
			[this, &transform](auto* object) { particleSystemEdit(&transform, object); });
		try_show_edit<FTerrainComponent>("Terrain", selected,
			[this, &transform](auto* object) { terrainEdit(object); });

		try_show_edit<FDirectionalLightComponent>("Directional light", selected,
			[&](auto* object)
			{
				ImGui::GColorEdit3("Color", object->color);
				ImGui::GDragFloat("Intencity", &object->intencity, 0.01f, 0.01f, 50.f);
				ImGui::GCheckbox("Cast shadows", &object->castShadows);

				auto direction = glm::normalize(glm::toQuat(transform.model) * glm::vec3(0.f, 0.f, 1.f));
				debug_draw->drawDebugArrow(transform.rposition, transform.rposition + direction, 0.2f, object->color);
			});

		try_show_edit<FSpotLightComponent>("Spot light", selected,
			[&](auto* object)
			{
				ImGui::GColorEdit3("Color", object->color);
				ImGui::GDragFloatVec3("Target", object->target, 0.01f);
				ImGui::GDragFloat("Intencity", &object->intencity, 0.01f, 0.01f, 50.f);
				ImGui::GDragFloat("Inner angle", &object->innerAngle, 0.01f, 0.01f, object->outerAngle);
				ImGui::GDragFloat("Outer angle", &object->outerAngle, 0.01f, object->innerAngle, 3.14f);
				ImGui::GCheckbox("To target", &object->toTarget);
				ImGui::GCheckbox("Cast shadows", &object->castShadows);

				auto direction = object->toTarget ? object->target : glm::normalize(glm::toQuat(transform.model) * glm::vec3(0.f, 0.f, 1.f));
				debug_draw->drawDebugArrow(transform.rposition, transform.rposition + direction, 0.2f, object->color);
			});
		
		try_show_edit<FPointLightComponent>("Point light", selected,
			[&](auto* object)
			{
				ImGui::GColorEdit3("Color", object->color);
				ImGui::GDragFloat("Intencity", &object->intencity, 0.01f, 0.01f, 50.f);
				ImGui::GDragFloat("Radius", &object->radius, 0.01f, 0.01f);
				ImGui::GCheckbox("Cast shadows", &object->castShadows);

				debug_draw->drawDebugSphere(transform.rposition, object->radius, object->color);
			});

		auto& plus = EGEditor->getIcon(icons::plus);
		if (ImGui::Button(plus.c_str(), ImVec2(-1.f, 0.f)))
			ImGui::OpenPopup("add_component_popup");

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const char* path = (const char*)payload->Data;
				auto source = std::string(path);

				if (fs::is_audio_format(source))
				{
					if (!registry.try_get<FAudioComponent>(selected))
					{
						FAudioComponent naudio{};
						naudio.source = source;

						registry.emplace<FAudioComponent>(selected, std::move(naudio));
					}
				}
				else if (fs::is_script_format(source))
				{
					if (!registry.try_get<FScriptComponent>(selected))
					{
						FScriptComponent nscript{};
						nscript.source = source;

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
						nskybox.source = source;

						registry.emplace<FEnvironmentComponent>(selected, std::move(nskybox));
					}
				}
				else if (fs::is_mesh_format(source))
				{
					if (!registry.try_get<FSceneComponent>(selected))
					{
						FSceneComponent nscene{};
						nscene.source = source;

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
			const char* path = (const char*)payload->Data;
			auto source = std::string(path);

			if (fs::is_audio_format(source))
			{
				if (object->source != source)
				{
					object->source = source;

					if (object->loaded)
					{
						EGAudio->removeSource(object->asource);
						auto pAudio = std::make_unique<CAudioSource>(object->source);
						object->asource = EGAudio->addSource(fs::get_filename(source), std::move(pAudio));
					}
					else
					{
						auto self = EGEditor->getLastSelection();
						auto& registry = EGEngine->getRegistry();

						FAudioComponent naudio{};
						naudio.source = source;

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
	auto& graphics = EGEngine->getGraphics();
	auto& debug_draw = graphics->getDebugDraw();

	ImGui::GDragFloat("FoV", &object->fieldOfView, 0.01f, 0.01f, 180.f);
	ImGui::GDragFloat("Near", &object->nearPlane, 0.01f, 0.01f, 1024.f);
	ImGui::GDragFloat("Far", &object->farPlane, 0.01f, 0.01f, 2048.f);
	ImGui::GDragFloat("Sensitivity", &object->sensitivity, 0.01f, 1.f, 50.f);
	ImGui::GCheckbox("Active", &object->active);
	ImGui::Separator();

	ImGui::Text("Post processing");
	ImGui::Separator();
	{
		{
			ImGui::PushID("fxaa");
			ImGui::Text("FXAA");
			ImGui::GCheckbox("enable", &object->effects.fxaa.enable);
			ImGui::GCombo("quality", &object->effects.fxaa.quality, fxaa_quality_variant.data(), 5);
			ImGui::GCombo("threshold", &object->effects.fxaa.threshold, fxaa_threshold_variant.data(), 5);
			ImGui::GCombo("threshold min", &object->effects.fxaa.threshold_min, fxaa_threshold_min_variant.data(), 3);
			ImGui::PopID();
		}

		ImGui::Separator();

		{
			ImGui::PushID("dof");
			ImGui::Text("Depth of Field");
			ImGui::GCheckbox("enable", &object->effects.dof.enable);
			ImGui::GDragInt("bokeh_samples", &object->effects.dof.bokeh_samples, 1, 2, 20);
			ImGui::GDragFloat("bokeh_poly", &object->effects.dof.bokeh_poly, 1.f, 1.f, 20.f);
			ImGui::GDragFloat("focus point", &object->effects.dof.focus_point, 0.01f, 0.01f, 100.f);
			ImGui::GDragFloat("near field", &object->effects.dof.near_field, 0.01f, 0.01f, object->effects.dof.far_field);
			ImGui::GDragFloat("near transition", &object->effects.dof.near_transition, 0.01f, 0.01f, 1.f);
			ImGui::GDragFloat("far field", &object->effects.dof.far_field, 0.01f, object->effects.dof.near_field, 100.f);
			ImGui::GDragFloat("far transition", &object->effects.dof.far_transition, 0.01f, 0.01f, 100.f);
			ImGui::PopID();
		}

		ImGui::Separator();

		{
			ImGui::PushID("bloom");
			ImGui::Text("Bloom");
			ImGui::GCheckbox("enable", &object->effects.bloom.enable);
			ImGui::GDragFloat("bloom_threshold", &object->effects.bloom.threshold, 0.01f, 0.01f, 1.f);
			ImGui::GDragFloat("radius", &object->effects.bloom.filter_radius, 0.001f, 0.001f, 0.01f);
			ImGui::GDragFloat("strength", &object->effects.bloom.strength, 0.01f, 0.01f, 0.2f);
			ImGui::PopID();
		}

		ImGui::Separator();

		{
			ImGui::PushID("aberration");
			ImGui::Text("Chromatic aberration");
			ImGui::GCheckbox("enable", &object->effects.aberration.enable);
			ImGui::GDragFloat("distortion", &object->effects.aberration.distortion, 0.01f, 0.01f, 10.f);
			ImGui::GDragInt("itteration", &object->effects.aberration.iterations, 3, 3, 24);
			ImGui::PopID();
		}

		ImGui::Separator();

		{
			ImGui::PushID("vignette");
			ImGui::Text("Vignette");
			ImGui::GCheckbox("enable", &object->effects.vignette.enable);
			ImGui::GDragFloat("inner", &object->effects.vignette.inner, 0.01f, 0.01f, object->effects.vignette.outer);
			ImGui::GDragFloat("outer", &object->effects.vignette.outer, 0.01f, object->effects.vignette.inner, 10.f);
			ImGui::GDragFloat("opacity", &object->effects.vignette.opacity, 0.01f, 0.01f, 1.f);
			ImGui::PopID();
		}

		ImGui::Separator();

		{
			ImGui::PushID("filmgrain");
			ImGui::Text("Film grain");
			ImGui::GCheckbox("enable", &object->effects.filmgrain.enable);
			ImGui::GDragFloat("amount", &object->effects.filmgrain.amount, 0.01f, 0.01f, 1.f);
			ImGui::PopID();
		}

		ImGui::Separator();

		{
			ImGui::PushID("fog");
			ImGui::Text("Fog");
			ImGui::GCheckbox("enable", &object->effects.fog.enable);
			ImGui::GColorEdit3("color", object->effects.fog.color);
			ImGui::GDragFloat("density", &object->effects.fog.density, 0.01f, 0.01f, 50.f);
			ImGui::PopID();
		}

		ImGui::Separator();

		{
			ImGui::PushID("tonemap");
			ImGui::Text("Tonemapping");
			ImGui::GCheckbox("enable", &object->effects.tonemap.enable);
			ImGui::GCheckbox("adaptive", &object->effects.tonemap.adaptive);
			ImGui::GDragFloat("gamma", &object->effects.tonemap.gamma, 0.01f, 0.01f, 4.f);

			if(!object->effects.tonemap.adaptive)
				ImGui::GDragFloat("exposure", &object->effects.tonemap.exposure, 0.01f, 0.01f, 11.f);
			else
			{
				ImGui::GDragFloat("Lum min", &object->effects.tonemap.lumMin, 0.01f, -50.f, object->effects.tonemap.lumMax);
				ImGui::GDragFloat("Lum max", &object->effects.tonemap.lumMax, 0.01f, 0.01f, 100.f);
				ImGui::GDragFloat("Tau", &object->effects.tonemap.tau, 0.01f, 0.01f, 10.f);
			}
				

			ImGui::GDragFloat("white point", &object->effects.tonemap.whitePoint, 0.01f, 0.01f, 11.f);
			
			ImGui::PopID();
		}
	}

	auto clip = object->frustum.getClipMatrix();
	auto clipMat = glm::make_mat4(clip.data());
	clipMat = glm::inverse(object->projection * object->view);
	debug_draw->drawDebugFrustum(clipMat);
}

// script editor
void CEditorInspector::scriptEdit(FScriptComponent* object)
{
	ImGui::GAssetHolder("Script", fs::get_filename(object->source));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const char* path = (const char*)payload->Data;
			auto source = std::string(path);

			if (fs::is_script_format(source))
			{
				object->source = source;
				if (object->loaded)
				{
					EGScripting->removeSource(object->data);
					EGScripting->addSource(object->source, object->source);
				}
				else
				{
					auto& registry = EGEngine->getRegistry();
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
	auto& registry = EGEngine->getRegistry();
	auto self = EGEditor->getLastSelection();

	ImGui::GAssetHolder("Source", fs::get_filename(object->source));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const char* path = (const char*)payload->Data;
			auto source = std::string(path);

			if (fs::is_skybox_format(source))
			{
				std::unique_ptr<FImageCreateInfo> pImageCI;
				CImageLoader::load(source, pImageCI);

				if (object->source != source && pImageCI->isCubemap)
				{
					object->source = source;
					if (object->loaded)
					{
						auto& graphics = EGEngine->getGraphics();
						graphics->removeImage(object->prefiltred);
						graphics->removeImage(object->irradiance);
						graphics->removeImage(object->origin);
						object->origin = graphics->addImage(object->source, object->source);
						object->irradiance = graphics->computeIrradiance(object->origin, 64);
						object->prefiltred = graphics->computePrefiltered(object->origin, 512);
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
	auto& registry = EGEngine->getRegistry();
	auto self = EGEditor->getLastSelection();

	ImGui::GAssetHolder("Source", fs::get_filename(object->source));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const char* path = (const char*)payload->Data;
			auto source = std::string(path);

			if (fs::is_mesh_format(source))
			{
				if (object->source != source)
				{
					object->source = source;
					if (object->loaded)
					{
						auto& hierarchy = registry.get<FHierarchyComponent>(self);
						while (!hierarchy.children.empty())
						{
							auto child = hierarchy.children.front();
							scenegraph::destroy_node(child);
						}

						CMeshLoader::load(object->source, self, object);
					}
					else
					{
						auto& registry = EGEngine->getRegistry();

						FSceneComponent nscene{};
						nscene.source = source;

						registry.remove<FSceneComponent>(self);
						registry.emplace<FSceneComponent>(self, std::move(nscene));
					}
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::GCheckbox("Cast Shadows", &object->castShadows);
}

void CEditorInspector::rigidbodyEdit(FTransformComponent* transform, FRigidBodyComponent* object)
{
	auto& graphics = EGEngine->getGraphics();
	auto& debug_draw = graphics->getDebugDraw();
	auto& registry = EGEngine->getRegistry();

	ImGui::GDragFloat("Mass", &object->mass, 0.1f, 0.f, INFINITE);
	if (ImGui::GCombo("Shape", &object->shape.type, collision_variant.data(), collision_variant.size()))
	{
		auto& physics = EGEngine->getPhysics();
		auto& pobject = physics->getObject(object->object_id);
		pobject->update(object);
	}

	auto& shape = object->shape;
	switch (static_cast<EPhysicsShapeType>(shape.type))
	{
	case EPhysicsShapeType::eBox: {
		ImGui::GDragFloatVec3("Size", shape.sizes);
		debug_draw->drawDebugBox(transform->rposition, shape.sizes.x * 2.f, shape.sizes.y * 2.f, shape.sizes.z * 2.f);

	} break;
	case EPhysicsShapeType::eCapsule: {
		ImGui::GDragFloat("Radius", &shape.radius, 0.01f, 0.f, 9999.f);
		ImGui::GDragFloat("Height", &shape.height, 0.01f, 0.f, 9999.f);
	} break;
	case EPhysicsShapeType::eCone: {
		ImGui::GDragFloat("Radius", &shape.radius, 0.01f, 0.f, 9999.f);
		ImGui::GDragFloat("Height", &shape.height, 0.01f, 0.f, 9999.f);
		debug_draw->drawDebugCone(transform->rposition, glm::normalize(transform->rposition + transform->rotation + shape.height), shape.radius, 0.f);
	} break;
	case EPhysicsShapeType::eCylinder: {
		ImGui::GDragFloatVec3("Size", shape.sizes);
		debug_draw->drawDebugCone(transform->rposition, glm::normalize(transform->rposition + transform->rotation), shape.sizes.x, shape.sizes.x);
	} break;
	case EPhysicsShapeType::eSphere: {
		ImGui::GDragFloat("Radius", &shape.radius, 0.01f, 0.f, 9999.f);
		debug_draw->drawDebugSphere(transform->rposition, shape.radius);
	} break;
	}
}

void CEditorInspector::particleSystemEdit(FTransformComponent* transform, FParticleComponent* object)
{
	auto self = EGEditor->getLastSelection();

	ImGui::GAssetHolder("Source", fs::get_filename(object->source));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const char* path = (const char*)payload->Data;
			auto source = std::string(path);

			if (fs::is_particle_format(source))
			{
				if (object->source != source)
				{
					object->source = source;
					if (object->loaded)
					{
						// Delete old
						// Add new
					}
					else
					{
						auto& registry = EGEngine->getRegistry();

						FParticleComponent nparticle{};
						nparticle.source = source;

						registry.remove<FParticleComponent>(self);
						registry.emplace<FParticleComponent>(self, std::move(nparticle));
					}
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::Text("");
}

void CEditorInspector::terrainEdit(FTerrainComponent* object)
{
	ImGui::GAssetHolder("Source", fs::get_filename(object->source));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			auto source = std::filesystem::path(path);

			//if (fs::is_particle_format(source))
			//{
			//	if (object->source != source)
			//	{
			//		object->source = fs::from_unicode(source);
			//		if (object->loaded)
			//		{
			//			// Delete old
			//			// Add new
			//		}
			//		else
			//		{
			//			auto& registry = EGEngine->getRegistry();
			//
			//			FParticleComponent nparticle{};
			//			nparticle.source = fs::from_unicode(source);
			//
			//			registry.remove<FParticleComponent>(self);
			//			registry.emplace<FParticleComponent>(self, std::move(nparticle));
			//		}
			//	}
			//}
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::GDragFloat("UV scale", &object->uv_scale, 0.1f, 0.1f, 5.f);

	if (ImGui::Button("Open material editor"))
	{
		CEvent eevent(Events::Editor::SelectMaterial);
		eevent.setParam(Events::Editor::SelectMaterial, object->material_id);
		EGEngine->sendEvent(eevent);
	}
}

void CEditorInspector::meshEdit(FMeshComponent* object)
{
	for (auto& meshlet : object->vMeshlets)
	{
		if (ImGui::Button("Open material editor"))
		{
			CEvent eevent(Events::Editor::SelectMaterial);
			eevent.setParam(Events::Editor::SelectMaterial, meshlet.material);
			EGEngine->sendEvent(eevent);
		}
	}
}