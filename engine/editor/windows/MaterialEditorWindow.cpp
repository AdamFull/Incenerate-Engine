#include "MaterialEditorWindow.h"

#include "Engine.h"
#include "system/filesystem/filesystem.h"

#include <imgui/imgui.h>
#include <imgui/imnodes.h>

using namespace engine::system;
using namespace engine::editor;

void CMaterialEditorWindow::create()
{
	//bIsOpen = false;
	EGEngine->addEventListener(Events::Editor::SelectMaterial, this, &CMaterialEditorWindow::onSetMaterial);

	fs::read_json("materialeditor.json", vGroups, true);

	pContainer = std::make_unique<CNodeAttribContainer>();
}

void CMaterialEditorWindow::__draw(float fDt)
{
	const bool open_popup = 
		ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
		//ImNodes::IsEditorHovered() &&
		ImGui::IsMouseClicked(ImGuiMouseButton_Right);

	int32_t hovered_node_id{ 0 }, hovered_link_id{ 0 }, hovered_pin_id{ 0 };
	auto isNodeHovered = ImNodes::IsNodeHovered(&hovered_node_id);
	auto isLinkHovered = ImNodes::IsLinkHovered(&hovered_link_id);
	auto isPinHovered = ImNodes::IsPinHovered(&hovered_pin_id);

	if (!ImGui::IsAnyItemHovered() && open_popup)
	{
		if (isNodeHovered)
		{
			hovered_id = hovered_node_id;
			ImGui::OpenPopup("node actions");
		}
		else if (isLinkHovered)
		{
			hovered_id = hovered_link_id;
			ImGui::OpenPopup("link actions");
		}
		else if (isPinHovered)
		{
			hovered_id = hovered_pin_id;
			ImGui::OpenPopup("pin actions");
		}
		else
			ImGui::OpenPopup("add node");
	}

	if (ImGui::BeginPopup("node actions"))
	{
		if (ImGui::MenuItem("delete"))
			pContainer->removeNode(hovered_id);
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("link actions"))
	{
		if (ImGui::MenuItem("break"))
			pContainer->removeLink(hovered_id);
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("pin actions"))
	{
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("add node"))
	{
		makePopupMenuContent(vGroups);
		ImGui::EndPopup();
	}

	ImNodes::BeginNodeEditor();
	pContainer->render();
	ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_TopRight);
	ImNodes::EndNodeEditor();

	int32_t link_from, link_to;
	if (ImNodes::IsLinkCreated(&link_from, &link_to))
		pContainer->createLink(link_from, link_to);

	int32_t link_id{ 0 };
	if (ImNodes::IsLinkDestroyed(&link_id))
		pContainer->removeLink(link_id);

	//auto& graphics = EGEngine->getGraphics();
	//if (selected_material != invalid_index)
	//{
	//	auto& material = graphics->getMaterial(selected_material);
	//	auto& params = material->getParameters();
	//
	//	//EAlphaMode alphaMode{ EAlphaMode::EOPAQUE };
	//	//ImGui::GDragFloat("Alpha cutoff", &params.alphaCutoff, 0.01f, 0.f, 1.f);
	//	//ImGui::GColorEdit3("Emissive factor", params.emissiveFactor);
	//	//ImGui::GDragFloat("Normal scale", &params.normalScale, 0.01f, 1.f, 10.f);
	//	//ImGui::GDragFloat("Occlusion strength", &params.occlusionStrenth, 0.01f, 0.f, 10.f);
	//	//ImGui::GColorEdit4("Base color factor", params.baseColorFactor);
	//	//ImGui::GDragFloat("Metallic factor", &params.metallicFactor, 0.01f, 0.f, 1.f);
	//	//ImGui::GDragFloat("Roughness factor", &params.roughnessFactor, 0.01f, 0.f, 1.f);
	//	//ImGui::GCheckbox("Double sided", &params.doubleSided);
	//	//
	//	//// Custom extension
	//	//ImGui::GDragFloat("Tessellation factor", &params.tessellationFactor, 0.01f, 0.f, 10.f);
	//	//ImGui::GDragFloat("Displacement strength", &params.displacementStrength, 0.01f, 0.01f, 100.f);
	//	//
	//	//// KHR_materials_clearcoat
	//	//ImGui::GDragFloat("Clearcoat Factor", &params.clearcoatFactor, 0.01f, 0.f, 1.f);
	//	//ImGui::GDragFloat("Clearcoat Roughness Factor", &params.clearcoatRoughnessFactor, 0.01f, 0.01f, 1.f);
	//	//
	//	//// KHR_materials_emissive_strength
	//	//ImGui::GDragFloat("Emissive Strength", &params.emissiveStrength, 0.01f, 1.f, 10.f);
	//	//
	//	//// KHR_materials_ior
	//	//ImGui::GDragFloat("IOR", &params.ior, 0.01f, 0.f, 2.f);
	//	//
	//	//// KHR_materials_iridescence
	//	//ImGui::GDragFloat("Iridescence Factor", &params.iridescenceFactor, 0.01f, 0.f, 1.f);
	//	//ImGui::GDragFloat("Iridescence IOR", &params.iridescenceIor, 0.01f, 0.f, 2.f);
	//	//ImGui::GDragFloat("Iridescence Thickness Minimum", &params.iridescenceThicknessMinimum, 1.f, 1.f, 1000.f);
	//	//ImGui::GDragFloat("Iridescence Thickness Maximum", &params.iridescenceThicknessMaximum, 1.f, 1.f, 1000.f);
	//	//
	//	//// KHR_materials_sheen
	//	//ImGui::GColorEdit3("Sheen Color Factor", params.sheenColorFactor);
	//	//ImGui::GDragFloat("Sheen Roughness Factor", &params.sheenRoughnessFactor, 0.01f, 0.f, 1.f);
	//	//
	//	//// KHR_materials_specular
	//	//ImGui::GDragFloat("Specular Factor", &params.specularFactor, 0.01f, 0.f, 1.f);
	//	//ImGui::GColorEdit3("Specular Color Factor", params.specularColorFactor);
	//	//
	//	//// KHR_materials_transmission
	//	//ImGui::GDragFloat("Transmission Factor", &params.transmissionFactor, 0.01f, 0.f, 1.f);
	//	//
	//	//// KHR_materials_unlit ?
	//	//
	//	//// KHR_materials_volume
	//	//ImGui::GDragFloat("Thickness Factor", &params.thicknessFactor, 0.01f, 0.f, 1.f);
	//	//ImGui::GDragFloat("Attenuation Distance", &params.attenuationDistance, 1.f, 0.f, INFINITY);
	//	//ImGui::GColorEdit3("Attenuation Color", params.attenuationColor);
	//}
}

void CMaterialEditorWindow::onSetMaterial(CEvent& event)
{
	bIsOpen = true;
	selected_material = event.getParam<size_t>(Events::Editor::SelectMaterial);
}

void CMaterialEditorWindow::makePopupMenuContent(const std::vector<FMaterialEditorGroupCreateInfo>& groups)
{
	const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
	for (auto& group : groups)
	{
		if (!group.subgroups.empty())
		{
			if (ImGui::BeginMenu(group.name.c_str()))
			{
				makePopupMenuContent(group.subgroups);
				ImGui::EndMenu();
			}
		}
		else
		{
			if (ImGui::BeginMenu(group.name.c_str()))
			{
				for (auto& node : group.nodes)
				{
					if (ImGui::MenuItem(node.name.c_str()))
						ImNodes::SetNodeScreenSpacePos(pContainer->createNode(node), click_pos);
				}
				ImGui::EndMenu();
			}
		}
	}
}