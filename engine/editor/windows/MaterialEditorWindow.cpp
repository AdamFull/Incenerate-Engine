#include "MaterialEditorWindow.h"

#include "Engine.h"

#include <imgui/imgui.h>
#include "editor/CustomControls.h"

using namespace engine::editor;

void CMaterialEditorWindow::create()
{
	bIsOpen = false;
	EGEngine->addEventListener(Events::Editor::SelectMaterial, this, &CMaterialEditorWindow::onSetMaterial);
}

void CMaterialEditorWindow::__draw(float fDt)
{
	auto& graphics = EGEngine->getGraphics();
	if (selected_material != invalid_index)
	{
		auto& material = graphics->getMaterial(selected_material);
		auto& params = material->getParameters();

		//EAlphaMode alphaMode{ EAlphaMode::EOPAQUE };
		ImGui::GDragFloat("Alpha cutoff", &params.alphaCutoff, 0.01f, 0.f, 1.f);
		ImGui::GColorEdit3("Emissive factor", params.emissiveFactor);
		ImGui::GDragFloat("Normal scale", &params.normalScale, 0.01f, 1.f, 10.f);
		ImGui::GDragFloat("Occlusion strength", &params.occlusionStrenth, 0.01f, 0.f, 10.f);
		ImGui::GColorEdit4("Base color factor", params.baseColorFactor);
		ImGui::GDragFloat("Metallic factor", &params.metallicFactor, 0.01f, 0.f, 1.f);
		ImGui::GDragFloat("Roughness factor", &params.roughnessFactor, 0.01f, 0.f, 1.f);
		ImGui::GCheckbox("Double sided", &params.doubleSided);

		// Custom extension
		ImGui::GDragFloat("Tessellation factor", &params.tessellationFactor, 0.01f, 0.f, 10.f);
		ImGui::GDragFloat("Displacement strength", &params.displacementStrength, 0.01f, 0.01f, 100.f);

		// KHR_materials_clearcoat
		ImGui::GDragFloat("Clearcoat Factor", &params.clearcoatFactor, 0.01f, 0.f, 1.f);
		ImGui::GDragFloat("Clearcoat Roughness Factor", &params.clearcoatRoughnessFactor, 0.01f, 0.01f, 1.f);

		// KHR_materials_emissive_strength
		ImGui::GDragFloat("Emissive Strength", &params.emissiveStrength, 0.01f, 1.f, 10.f);

		// KHR_materials_ior
		ImGui::GDragFloat("IOR", &params.ior, 0.01f, 0.f, 2.f);

		// KHR_materials_iridescence
		ImGui::GDragFloat("Iridescence Factor", &params.iridescenceFactor, 0.01f, 0.f, 1.f);
		ImGui::GDragFloat("Iridescence IOR", &params.iridescenceIor, 0.01f, 0.f, 2.f);
		ImGui::GDragFloat("Iridescence Thickness Minimum", &params.iridescenceThicknessMinimum, 1.f, 1.f, 1000.f);
		ImGui::GDragFloat("Iridescence Thickness Maximum", &params.iridescenceThicknessMaximum, 1.f, 1.f, 1000.f);

		// KHR_materials_sheen
		ImGui::GColorEdit3("Sheen Color Factor", params.sheenColorFactor);
		ImGui::GDragFloat("Sheen Roughness Factor", &params.sheenRoughnessFactor, 0.01f, 0.f, 1.f);

		// KHR_materials_specular
		ImGui::GDragFloat("Specular Factor", &params.specularFactor, 0.01f, 0.f, 1.f);
		ImGui::GColorEdit3("Specular Color Factor", params.specularColorFactor);

		// KHR_materials_transmission
		ImGui::GDragFloat("Transmission Factor", &params.transmissionFactor, 0.01f, 0.f, 1.f);

		// KHR_materials_unlit ?

		// KHR_materials_volume
		ImGui::GDragFloat("Thickness Factor", &params.thicknessFactor, 0.01f, 0.f, 1.f);
		ImGui::GDragFloat("Attenuation Distance", &params.attenuationDistance, 1.f, 0.f, INFINITY);
		ImGui::GColorEdit3("Attenuation Color", params.attenuationColor);
	}
}

void CMaterialEditorWindow::onSetMaterial(CEvent& event)
{
	bIsOpen = true;
	selected_material = event.getParam<size_t>(Events::Editor::SelectMaterial);
}