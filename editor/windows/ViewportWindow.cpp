#include "ViewportWindow.h"

#include "Editor.h"
#include "Engine.h"

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>
#include "graphics/imgui_impl/imgui_impl_vulkan.h"

#include "ecs/components/components.h"
#include "ecs/helper.hpp"

#include "operations/PropertyChangedOperation.h"

#include <Helpers.h>

using namespace engine::ecs;
using namespace engine::editor;
using namespace engine::graphics;

static const float identityMatrix[16] =
{ 1.f, 0.f, 0.f, 0.f,
0.f, 1.f, 0.f, 0.f,
0.f, 0.f, 1.f, 0.f,
0.f, 0.f, 0.f, 1.f };

ImGuizmo::OPERATION mCurrentGizmoOperation{ ImGuizmo::TRANSLATE };
ImGuizmo::MODE mCurrentGizmoMode{ ImGuizmo::LOCAL };

void AlignForWidth(float width, float alignment = 0.5f)
{
	ImGuiStyle& style = ImGui::GetStyle();
	float avail = ImGui::GetContentRegionAvail().x;
	float off = (avail - width) * alignment;
	if (off > 0.0f)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}

CEditorViewport::~CEditorViewport()
{

}

void CEditorViewport::create()
{
	graphics = EGEngine->getGraphics().get();
	auto& device = graphics->getDevice();

	auto pBackend = (ImGui_ImplVulkan_Data*)ImGui::GetIO().BackendRendererUserData;
	std::unordered_map<uint32_t, vk::DescriptorSetLayout> descriptorSetLayouts{ {0, pBackend->DescriptorSetLayout} };

	pDescriptorSet = std::make_unique<CDescriptorSet>(device.get());
	pDescriptorSet->create(EGEditor->getDescriptorPool(), descriptorSetLayouts);
}

void CEditorViewport::__draw(float fDt)
{
	auto& graphics = EGEngine->getGraphics();
	auto& debug_draw = graphics->getDebugDraw();

	viewportPanelSizeX = ImGui::GetContentRegionAvail().x;
	viewportPanelSizeY = ImGui::GetContentRegionAvail().y;

	ImVec2 viewportPanelPos = ImGui::GetWindowPos();
	auto textDrawPos = ImGui::GetCursorStartPos();

	drawViewport();

	auto controlls_enable = ImGui::IsWindowHovered(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::IsWindowFocused();

	if (auto camera = EGEngine->getActiveCamera())
		camera->controllable = controlls_enable;

	if (EGEngine->isEditorEditing())
	{
		viewportPicking();
		drawManipulator(viewportPanelPos.x, viewportPanelPos.y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
	}

	drawOverlay(textDrawPos.x, textDrawPos.y, fDt);
}

void CEditorViewport::drawViewport()
{
	auto& device = graphics->getDevice();
	auto& image = graphics->getImage("postprocess_tex");
	
	vk::WriteDescriptorSet write{};
	write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
	write.dstBinding = 0;
	write.pImageInfo = &image->getDescriptor();
	write.descriptorCount = 1;
	pDescriptorSet->update(write);

	ImGui::Image(pDescriptorSet->get(), ImVec2{ viewportPanelSizeX, viewportPanelSizeY });

	if (!ImGui::IsAnyItemActive())
		device->setViewportExtent(vk::Extent2D{(uint32_t)viewportPanelSizeX, (uint32_t)viewportPanelSizeY });
}

void CEditorViewport::viewportPicking()
{
	auto& device = graphics->getDevice();
	auto frame = device->getCurrentFrame();
	auto image_name = "picking_tex_" + std::to_string(frame);
	auto image_id = graphics->getImageID(image_name);
	auto& image = graphics->getImage(image_id);
	auto extent = image->getExtent();

	auto mousePos = ImGui::GetMousePos();
	mousePos.x = mousePos.x - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX();
	mousePos.y = (mousePos.y - ImGui::GetCursorScreenPos().y - ImGui::GetScrollY()) * -1.f;

	auto mouse_pos_x = glm::clamp(static_cast<uint32_t>(mousePos.x), 0u, extent.width);
	auto mouse_pos_y = extent.height - glm::clamp(static_cast<uint32_t>(mousePos.y), 0u, extent.height);

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGuizmo::IsOver() && ImGui::IsWindowFocused())
	{
		uint8_t pixel[4];
		device->readPixel(image_id, mouse_pos_x, mouse_pos_y, pixel);
		auto decoded = decodeIdFromColor(pixel[0], pixel[1], pixel[2], pixel[3]);
		log_debug("X: {} , Y: {}, decoded: {})", mouse_pos_x, mouse_pos_y, decoded);
		EGEditor->selectObject(static_cast<entt::entity>(decoded));
	}
}

void CEditorViewport::drawManipulator(float offsetx, float offsety, float sizex, float sizey)
{
	static glm::vec3 oldTranslation{ 0.f };
	static glm::quat oldRotation{ 1.f, 0.f, 0.f, 0.f };
	static glm::vec3 oldScale{ 1.f };

	if (ImGui::IsWindowHovered() && ImGui::IsWindowFocused())
	{
		if (ImGui::IsKeyPressed(ImGuiKey_T))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(ImGuiKey_R))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(ImGuiKey_E))
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (ImGui::IsKeyReleased(ImGuiKey_M))
			mCurrentGizmoMode = mCurrentGizmoMode == ImGuizmo::LOCAL ? ImGuizmo::WORLD : ImGuizmo::LOCAL;
	}

	auto selected = EGEditor->getLastSelection();
	if (selected != entt::null)
	{
		auto& registry = EGEngine->getRegistry();

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

		//Manipulating
		if (ImGuizmo::Manipulate(glm::value_ptr(camera->view), glm::value_ptr(camera->projection), mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(transform.model), glm::value_ptr(delta)))
		{
			glm::quat rotation{};
			glm::vec3 translation, scale, skew;
			glm::vec4 perspective{};
			glm::decompose(delta, scale, rotation, translation, skew, perspective);

			if (!isManipulating)
			{
				isManipulating = true;
				oldTranslation = transform.position;
				oldRotation = transform.rotation;
				oldScale = transform.scale;
			}

			switch (mCurrentGizmoOperation)
			{
			case ImGuizmo::TRANSLATE: { transform.position += translation; } break;
			case ImGuizmo::ROTATE: { transform.rotation += rotation; } break;
			case ImGuizmo::SCALE: { transform.scale *= scale; } break;
			}

			return;
		}

		if (isManipulating && !ImGuizmo::IsOver())
		{
			auto& actionBuffer = EGEditor->getActionBuffer();

			switch (mCurrentGizmoOperation)
			{
			case ImGuizmo::TRANSLATE:
				if(oldTranslation != transform.position)
					actionBuffer->addOperation(std::make_unique<CPropertyChangedOperation>(oldTranslation, &transform.position, utl::type_hash<glm::vec3>()));
				break;
			case ImGuizmo::ROTATE:
				if(oldRotation != transform.rotation)
					actionBuffer->addOperation(std::make_unique<CPropertyChangedOperation>(oldRotation, &transform.rotation, utl::type_hash<glm::vec3>()));
				break;
			case ImGuizmo::SCALE:
				if(oldScale != transform.scale)
					actionBuffer->addOperation(std::make_unique<CPropertyChangedOperation>(oldScale, &transform.scale, utl::type_hash<glm::vec3>()));
				break;
			default:
				break;
			}

			isManipulating = false;
		}
	}
}

void CEditorViewport::drawOverlay(float offsetx, float offsety, float fDt)
{
	ImGui::SetCursorPos(ImVec2(offsetx, offsety));
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.23529413f, 0.24705884f, 0.25490198f, 0.50f });
	ImGui::BeginChild("ChildR", ImVec2(viewportPanelSizeX, overlayY), false, window_flags);

	if (frameRateAccum > 1.f)
	{
		frameRateAccum = 0.f;
		frameRate = frameRateCounter;
		frameRateCounter = 0;
	}
	else
	{
		frameRateAccum += fDt;
		frameRateCounter += 1;
	}

	auto& io = ImGui::GetIO();

	// Fps / frame time
	char overlay[64];
	sprintf(overlay, "dt: %.3f | FPS: %u", fDt, frameRate);
	ImGui::Text(overlay);

	sprintf(overlay, "Draw calls: %u", graphics->getDrawCallCount());
	ImGui::Text(overlay);

	// GPU name
	auto& physical = graphics->getDevice()->getPhysical();
	auto props = physical.getProperties();
	sprintf(overlay, "GPU: %s", props.deviceName.data());
	ImGui::Text(overlay);



	overlayY = ImGui::GetCursorPosY();

	ImGui::SetCursorPosY(0);

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
	auto disableStop = state == EEngineState::eEditing;
	auto disablePlay = state == EEngineState::ePlay;

	// TODO: should pause any simulation
	ImGui::BeginDisabled(disablePause);
	if (ImGui::Button(pause_icon.c_str()))
		EGEngine->setState(EEngineState::ePause);
	ImGui::EndDisabled();

	ImGui::SameLine();

	ImGui::BeginDisabled(disablePlay);
	if (ImGui::Button(play_icon.c_str()))
	{
		auto& physics = EGEngine->getPhysics();
		physics->reset();

		EGEditor->captureSceneState();
		EGEditor->activateSceneCamera();
		EGEngine->setState(EEngineState::ePlay);
	}
	ImGui::EndDisabled();
		
	ImGui::SameLine();

	ImGui::BeginDisabled(disableStop);
	if (ImGui::Button(stop_icon.c_str()))
	{
		EGEditor->restoreSceneState();
		EGEditor->activateEditorCamera();
		EGEngine->setState(EEngineState::eEditing);
	}
	ImGui::EndDisabled();

	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}