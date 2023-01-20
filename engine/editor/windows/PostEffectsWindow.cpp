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
		ImGui::GDragFloat("qualitySubpix", &peffects.qualitySubpix, 0.01f, 0.01f, 1.f);
		ImGui::GDragFloat("qualityEdgeThreshold", &peffects.qualityEdgeThreshold, 0.001f, 0.001f, 1.f);
		ImGui::GDragFloat("qualityEdgeThresholdMin", &peffects.qualityEdgeThresholdMin, 0.001f, 0.001f, 1.f);
	}

	if (ImGui::CollapsingHeader("Tonemapping", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::GDragFloat("gamma", &peffects.gamma, 0.01f, 0.01f, 4.f);
		ImGui::GDragFloat("exposure", &peffects.exposure, 0.01f, 0.01f, 11.f);
	}

	if (ImGui::CollapsingHeader("Bloom", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::GDragFloat("bloom_threshold", &peffects.bloom_threshold, 0.01f, 0.01f, 1.f);
		ImGui::GDragFloat("filterRadius", &peffects.filterRadius, 0.001f, 0.001f, 0.01f);
		ImGui::GDragFloat("bloomStrength", &peffects.bloomStrength, 0.01f, 0.01f, 0.1f);
	}
}