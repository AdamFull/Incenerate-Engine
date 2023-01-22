#include "PostEffectsWindow.h"

#include "Engine.h"

#include <imgui/imgui.h>
#include "editor/CustomControls.h"

using namespace engine::editor;

void CEditorPostEffects::create()
{

}

void CEditorPostEffects::__draw(float fDt)
{
	auto& peffects = EGEngine->getPostEffects();

	if (ImGui::CollapsingHeader("FXAA", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::GCheckbox("enable", &peffects.fxaa);
		ImGui::GDragFloat("quality", &peffects.fxaa_quality, 0.01f, 0.01f, 1.f);
	}

	if (ImGui::CollapsingHeader("Depth of field", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::GCheckbox("enable", &peffects.dof);
		ImGui::GDragFloat("focus point", &peffects.dof_focus_point, 0.01f, 0.01f, 100.f);
		ImGui::GDragFloat("near field", &peffects.dof_near_field, 0.01f, 0.01f, 100.f);
		ImGui::GDragFloat("near transition", &peffects.dof_near_transition, 0.01f, 0.01f, 1.f);
		ImGui::GDragFloat("far field", &peffects.dof_far_field, 0.01f, 0.01f, 100.f);
		ImGui::GDragFloat("far transition", &peffects.dof_far_transition, 0.01f, 0.01f, 100.f);
	}

	if (ImGui::CollapsingHeader("Tonemapping", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::GCheckbox("enable", &peffects.tonemapping);
		ImGui::GDragFloat("gamma", &peffects.tonemapping_gamma, 0.01f, 0.01f, 4.f);
		ImGui::GDragFloat("exposure", &peffects.tonemapping_exposure, 0.01f, 0.01f, 11.f);
	}

	if (ImGui::CollapsingHeader("Bloom", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::GCheckbox("enable", &peffects.bloom);
		ImGui::GDragFloat("bloom_threshold", &peffects.bloom_threshold, 0.01f, 0.01f, 1.f);
		ImGui::GDragFloat("radius", &peffects.bloom_filter_radius, 0.001f, 0.001f, 0.01f);
		ImGui::GDragFloat("strength", &peffects.bloom_strength, 0.01f, 0.01f, 0.2f);
	}

	if (ImGui::CollapsingHeader("Chromatic aberration", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::GCheckbox("enable", &peffects.aberration);
		ImGui::GDragFloat("distortion", &peffects.aberration_distortion, 0.01f, 0.01f, 10.f);
		ImGui::GDragInt("itteration", &peffects.aberration_iterations, 3, 3, 24);
	}

	if (ImGui::CollapsingHeader("Vignette", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::GCheckbox("enable", &peffects.vignette);
		ImGui::GDragFloat("inner", &peffects.vignette_inner, 0.01f, 0.01f, 10.f);
		ImGui::GDragFloat("outer", &peffects.vignette_outer, 0.01f, 0.01f, 10.f);
		ImGui::GDragFloat("opacity", &peffects.vignette_opacity, 0.01f, 0.01f, 1.f);
	}
}