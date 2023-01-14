#include "ImageViewerWindow.h"

#include "Engine.h"

#include <imgui/imgui.h>
#include <editor/imgui_impl_vulkan.h>

using namespace engine::graphics;
using namespace engine::editor;

// TODO: Add event Events::Editor::OpenImageViewer

void CEditorImageViewer::create()
{
	auto pBackend = (ImGui_ImplVulkan_Data*)ImGui::GetIO().BackendRendererUserData;
	pDescriptorSet = std::make_unique<CDescriptorSet>(EGGraphics->getDevice().get());
	pDescriptorSet->create(vk::PipelineBindPoint::eGraphics, pBackend->PipelineLayout, EGEditor->getDescriptorPool(), pBackend->DescriptorSetLayout);
}

void CEditorImageViewer::__draw(float fDt)
{
	if (openned_image != invalid_index)
	{
		auto& pImage = EGGraphics->getImage(openned_image);

		// Draw image centred with original aspect
	}
}