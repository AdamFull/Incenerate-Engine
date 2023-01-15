#include "ViewportWindow.h"

#include "Engine.h"

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>
#include <editor/imgui_impl_vulkan.h>

#include "ecs/components/TransformComponent.h"
#include "ecs/components/HierarchyComponent.h"
#include "ecs/helper.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/epsilon.hpp>

using namespace engine::ecs;
using namespace engine::editor;
using namespace engine::graphics;

static const float identityMatrix[16] =
{ 1.f, 0.f, 0.f, 0.f,
0.f, 1.f, 0.f, 0.f,
0.f, 0.f, 1.f, 0.f,
0.f, 0.f, 0.f, 1.f };

ImGuizmo::OPERATION mCurrentGizmoOperation{ ImGuizmo::TRANSLATE };

void AlignForWidth(float width, float alignment = 0.5f)
{
	ImGuiStyle& style = ImGui::GetStyle();
	float avail = ImGui::GetContentRegionAvail().x;
	float off = (avail - width) * alignment;
	if (off > 0.0f)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}

bool decompose(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
{
	using T = float;
	glm::mat4 LocalMatrix(transform);

	// Normalize the matrix.
	if (glm::epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), glm::epsilon<T>()))
		return false;

	// First, isolate perspective.  This is the messiest.
	if (
		glm::epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), glm::epsilon<T>()) ||
		glm::epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), glm::epsilon<T>()) ||
		glm::epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), glm::epsilon<T>()))
	{
		// Clear the perspective partition
		LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
		LocalMatrix[3][3] = static_cast<T>(1);
	}

	// Next take care of translation (easy).
	translation = glm::vec3(LocalMatrix[3]);
	LocalMatrix[3] = glm::vec4(0, 0, 0, LocalMatrix[3].w);

	glm::vec3 Row[3], Pdum3;

	// Now get scale and shear.
	for (glm::length_t i = 0; i < 3; ++i)
		for (glm::length_t j = 0; j < 3; ++j)
			Row[i][j] = LocalMatrix[i][j];

	// Compute X scale factor and normalize first row.
	scale.x = glm::length(Row[0]);
	Row[0] = glm::detail::scale(Row[0], static_cast<T>(1));
	scale.y = glm::length(Row[1]);
	Row[1] = glm::detail::scale(Row[1], static_cast<T>(1));
	scale.z = glm::length(Row[2]);
	Row[2] = glm::detail::scale(Row[2], static_cast<T>(1));

	rotation.y = glm::asin(-Row[0][2]);
	if (glm::cos(rotation.y) != 0) 
	{
		rotation.x = atan2(Row[1][2], Row[2][2]);
		rotation.z = atan2(Row[0][1], Row[0][0]);
	}
	else 
	{
		rotation.x = atan2(-Row[2][0], Row[1][1]);
		rotation.z = 0;
	}


	return true;
}

CEditorViewport::~CEditorViewport()
{

}

void CEditorViewport::create()
{
	auto pBackend = (ImGui_ImplVulkan_Data*)ImGui::GetIO().BackendRendererUserData;
	pDescriptorSet = std::make_unique<CDescriptorSet>(EGGraphics->getDevice().get());
	pDescriptorSet->create(vk::PipelineBindPoint::eGraphics, pBackend->PipelineLayout, EGEditor->getDescriptorPool(), pBackend->DescriptorSetLayout);
}

void CEditorViewport::__draw(float fDt)
{
	auto& registry = EGCoordinator;
	auto editorMode = EGEngine->isEditorMode();
	auto state = EGEngine->getState();

	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	ImVec2 viewportPanelPos = ImGui::GetWindowPos();
	auto textDrawPos = ImGui::GetCursorStartPos();

	drawViewport(viewportPanelSize.x, viewportPanelSize.y);

	if (editorMode && state == EEngineState::eEditing)
	{
		if (auto camera = registry.try_get<FCameraComponent>(EGEditor->getCamera()))
			camera->controllable = ImGui::IsWindowHovered(ImGuiFocusedFlags_RootAndChildWindows);

		drawManipulator(viewportPanelPos.x, viewportPanelPos.y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
		drawOverlay(textDrawPos.x, textDrawPos.y, fDt);
	}
}

void CEditorViewport::drawViewport(float offsetx, float offsety)
{
	auto& device = EGGraphics->getDevice();
	auto frame = EGGraphics->getDevice()->getCurrentFrame();
	auto& image = EGGraphics->getImage("postprocess_tex_" + std::to_string(frame));
	
	vk::WriteDescriptorSet write{};
	write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
	write.dstBinding = 0;
	write.pImageInfo = &image->getDescriptor();
	write.descriptorCount = 1;
	pDescriptorSet->update(write);

	auto& pause_icon = EGEditor->getIcon(icons::pause);
	auto& play_icon = EGEditor->getIcon(icons::play);
	auto& stop_icon = EGEditor->getIcon(icons::stop);

	ImGuiStyle& style = ImGui::GetStyle();
	float width = 0.0f;
	width += ImGui::CalcTextSize(pause_icon.c_str()).x;
	width += style.ItemSpacing.x;
	width += ImGui::CalcTextSize(play_icon.c_str()).x;
	width += style.ItemSpacing.x;
	width += ImGui::CalcTextSize(stop_icon.c_str()).x;
	AlignForWidth(width);

	auto state = EGEngine->getState();
	auto disablePause = state == EEngineState::ePause || state == EEngineState::eEditing;

	if (disablePause)
		ImGui::BeginDisabled();

	if (ImGui::Button(pause_icon.c_str()))
		EGEngine->setState(EEngineState::ePause);

	if (disablePause)
		ImGui::EndDisabled();

	ImGui::SameLine();
	if (ImGui::Button(play_icon.c_str()))
		EGEngine->setState(EEngineState::ePlay);
	ImGui::SameLine();
	if (ImGui::Button(stop_icon.c_str()))
		EGEngine->setState(EEngineState::eEditing);

	offsety = ImGui::GetContentRegionAvail().y;

	ImGui::Image(pDescriptorSet->get(), ImVec2{ offsetx, offsety });

	if (!ImGui::IsAnyItemActive())
		device->setViewportExtent(vk::Extent2D{(uint32_t)offsetx, (uint32_t)offsety});
}

void CEditorViewport::drawManipulator(float offsetx, float offsety, float sizex, float sizey)
{
	bool snap{ false };

	if (ImGui::IsKeyPressed(ImGuiKey_T))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(ImGuiKey_R))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(ImGuiKey_E))
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::IsKeyPressed(ImGuiKey_V))
		snap = true;

	float snapValue = 0.5f;
	if (mCurrentGizmoOperation == ImGuizmo::OPERATION::ROTATE)
		snapValue = 45.0f;
	float snapValues[3] = { snapValue, snapValue, snapValue };

	auto selected = EGEditor->getLastSelection();
	if (selected != entt::null)
	{
		auto& registry = EGCoordinator;

		auto camera = registry.try_get<FCameraComponent>(EGEditor->getCamera());

		FTransformComponent* ptransform{ nullptr };

		auto& transform = registry.get<FTransformComponent>(selected);
		auto& hierarchy = registry.get<FHierarchyComponent>(selected);

		if(hierarchy.parent != entt::null)
			ptransform = registry.try_get<FTransformComponent>(hierarchy.parent);

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(offsetx, offsety, sizex, sizey);

		glm::mat4 delta;

		//if (ptransform)
		//	delta = ptransform->model;

		//Manipulating
		if (ImGuizmo::Manipulate(glm::value_ptr(camera->view), glm::value_ptr(camera->projection), mCurrentGizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(transform.model), glm::value_ptr(delta), snap ? snapValues : nullptr))
		{
			glm::vec3 translation, rotation, scale;
			decompose(delta, translation, rotation, scale);

			auto rel = glm::abs(translation - transform.position);
			auto moved = rel.x > 0.05f | rel.y > 0.05f | rel.z > 0.05f;

			switch (mCurrentGizmoOperation)
			{
			case ImGuizmo::TRANSLATE: transform.position += translation; break;
			case ImGuizmo::ROTATE: transform.rotation += rotation; break;
			case ImGuizmo::SCALE: transform.scale *= scale; break;
			}
		}
	}
}

void CEditorViewport::drawOverlay(float offsetx, float offsety, float fDt)
{
	ImGui::SetCursorPos(ImVec2(offsetx, offsety));
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.23529413f, 0.24705884f, 0.25490198f, 0.50f });
	ImGui::BeginChild("ChildR", ImVec2(overlayX, overlayY), false, window_flags);

	auto& io = ImGui::GetIO();
	char overlay[64];
	sprintf(overlay, "dt: %.3f | FPS: %.3f", fDt, 1.f / fDt);
	ImGui::Text(overlay);

	auto& physical = EGGraphics->getDevice()->getPhysical();
	auto props = physical.getProperties();
	sprintf(overlay, "GPU: %s", props.deviceName.data());
	ImGui::Text(overlay);

	//overlayX = ImGui::GetCursorPosX();
	overlayY = ImGui::GetCursorPosY();

	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}