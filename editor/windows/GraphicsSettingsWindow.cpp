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
}