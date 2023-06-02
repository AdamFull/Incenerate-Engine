#include "GraphicsSettingsWindow.h"

#include "CustomControls.h"

#include "graphics/GraphicsSettings.h"

using namespace engine::editor;
using namespace engine::graphics;

void CGraphicsSettingsWindow::create()
{

}

void CGraphicsSettingsWindow::__draw(float fDt)
{
	auto& settings = CGraphicsSettings::getInstance()->getSettings();

	ImGui::GCheckbox("Ambient occlusion", &settings.bEnableAmbientOcclusion);

	if (settings.bEnableAmbientOcclusion)
	{
		ImGui::GDragFloat("radius", &settings.fAmbientOcclusionRadius, 0.01f, 0.01f, 1.f);
		ImGui::GDragFloat("bias", &settings.fAmbientOcclusionBias, 0.001f, 0.001f, 0.05f);
		ImGui::GDragInt("smoothing", &settings.iAmbientOcclusionSmoothingSteps, 1, 1, 4);
	}


	ImGui::GCheckbox("Reflections", &settings.bEnableReflections);

	if (settings.bEnableReflections)
	{
		ImGui::GDragFloat("ray step", &settings.fReflectionRayStep, 0.01f, 0.01f, 1.f);
		ImGui::GDragInt("iteration count", &settings.iReflectionIterationCount, 1, 1, 500);
		ImGui::GDragFloat("bias", &settings.fReflectionDistanceBias, 0.01f, 0.01f, 1.f);

		ImGui::GCheckbox("sampling", &settings.bReflectionSamplingEnabled);
		if (settings.bReflectionSamplingEnabled)
		{
			ImGui::GDragInt("sample count", &settings.iReflectionSampleCount, 1, 1, 16);
			ImGui::GDragFloat("sampling coeff", &settings.fReflectionSamplingCoefficient, 0.01f, 0.0f, 0.5f);
		}

		ImGui::GCheckbox("binary search", &settings.bReflectionBinarySearchEnabled);

		ImGui::GCheckbox("adaptive step", &settings.bReflectionAdaptiveStepEnabled);
		ImGui::GCheckbox("exponential step", &settings.bReflectionExponentialStepEnabled);

		if (settings.bReflectionBinarySearchEnabled || settings.bReflectionExponentialStepEnabled)
			settings.bReflectionAdaptiveStepEnabled = false;

		if (settings.bReflectionAdaptiveStepEnabled)
			settings.bReflectionBinarySearchEnabled = false;

		ImGui::GCheckbox("debug", &settings.bReflectionDebugDraw);
	}
}