#include "ViewportWindow.h"

#include "Engine.h"

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>
#include <editor/imgui_impl_vulkan.h>

#include "ecs/components/TransformComponent.h"
#include "ecs/helper.hpp"

#include <glm/gtc/type_ptr.hpp>

using namespace engine::ecs;
using namespace engine::editor;
using namespace engine::graphics;

static const float identityMatrix[16] =
{ 1.f, 0.f, 0.f, 0.f,
0.f, 1.f, 0.f, 0.f,
0.f, 0.f, 1.f, 0.f,
0.f, 0.f, 0.f, 1.f };

ImGuizmo::OPERATION mCurrentGizmoOperation{ ImGuizmo::TRANSLATE };

CEditorViewport::~CEditorViewport()
{

}

void CEditorViewport::create()
{
	auto pBackend = (ImGui_ImplVulkan_Data*)ImGui::GetIO().BackendRendererUserData;
	pDescriptorSet = std::make_unique<CDescriptorSet>(EGGraphics->getDevice().get());
	pDescriptorSet->create(vk::PipelineBindPoint::eGraphics, pBackend->PipelineLayout, EGEditor->getDescriptorPool(), pBackend->DescriptorSetLayout);
}

void CEditorViewport::draw()
{
	if (bIsOpen)
	{
		if (ImGui::Begin("Viewport", &bIsOpen))
		{
			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			ImVec2 viewportPanelPos = ImGui::GetWindowPos();
			auto textDrawPos = ImGui::GetCursorStartPos();

			drawViewport(viewportPanelSize.x, viewportPanelSize.y);
			drawManipulator(viewportPanelPos.x, viewportPanelPos.y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
			drawOverlay(textDrawPos.x, textDrawPos.y);

			ImGui::End();
			ImGui::PopStyleVar();
		}
	}
}

void CEditorViewport::drawViewport(float offsetx, float offsety)
{
	auto frame = EGGraphics->getDevice()->getCurrentFrame();
	auto& image = EGGraphics->getImage("output_image_" + std::to_string(frame));
	
	vk::WriteDescriptorSet write{};
	write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
	write.dstBinding = 0;
	write.pImageInfo = &image->getDescriptor();
	write.descriptorCount = 1;
	pDescriptorSet->update(write);

	ImGui::Image(pDescriptorSet->get(), ImVec2{ offsetx, offsety });

	if (!ImGui::IsAnyItemActive())
		EGGraphics->getDevice()->setViewportExtent(vk::Extent2D{(uint32_t)offsetx, (uint32_t)offsety});
}

void CEditorViewport::drawManipulator(float offsetx, float offsety, float sizex, float sizey)
{
	bool snap{ false };

	if (ImGui::IsKeyPressed('T'))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed('R'))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed('E'))
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::IsKeyPressed('Z'))
		snap = true;

	float snapValue = 0.5f;
	if (mCurrentGizmoOperation == ImGuizmo::OPERATION::ROTATE)
		snapValue = 45.0f;
	float snapValues[3] = { snapValue, snapValue, snapValue };

	auto selected = EGEditor->getLastSelection();
	if (selected != entt::null)
	{
		auto& registry = EGCoordinator;
		auto ecamera = get_active_camera(registry);
		auto& camera = registry.get<FCameraComponent>(ecamera);

		auto& transform = registry.get<FTransformComponent>(selected);

		//camera->setControl(ImGui::IsWindowHovered(ImGuiFocusedFlags_RootAndChildWindows));

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(offsetx, offsety, sizex, sizey);

		glm::mat4 delta;

		//Manipulating
		if (ImGuizmo::Manipulate(glm::value_ptr(camera.view), glm::value_ptr(camera.projection), mCurrentGizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(transform.model), glm::value_ptr(delta), nullptr))
		{
			glm::vec3 scale;
			glm::quat rotation;
			glm::vec3 translation;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(delta, scale, rotation, translation, skew, perspective);

			switch (mCurrentGizmoOperation)
			{
			case ImGuizmo::TRANSLATE: transform.position += translation; break;
			case ImGuizmo::ROTATE: transform.rotation += glm::eulerAngles(glm::conjugate(rotation)); break;
			case ImGuizmo::SCALE: transform.scale *= scale; break;
			}
		}
	}
}

void CEditorViewport::drawOverlay(float offsetx, float offsety)
{
	ImGui::SetCursorPos(ImVec2(offsetx, offsety));
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.23529413f, 0.24705884f, 0.25490198f, 0.50f });
	ImGui::BeginChild("ChildR", ImVec2(overlayX, overlayY), false, window_flags);

	auto& io = ImGui::GetIO();
	char overlay[64];
	float fFrameTime = 1000.0f / io.Framerate;
	sprintf(overlay, "dt: %.3f | FPS: %.3f", fFrameTime, io.Framerate);
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