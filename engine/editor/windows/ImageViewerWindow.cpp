#include "ImageViewerWindow.h"

#include "Engine.h"
#include "system/filesystem/filesystem.h"

#include <imgui/imgui.h>
#include <editor/imgui_impl_vulkan.h>

using namespace engine::graphics;
using namespace engine::system;
using namespace engine::editor;

// TODO: Add event Events::Editor::OpenImageViewer

void CEditorImageViewer::create()
{
	bIsOpen = false;

	graphics = EGEngine->getGraphics().get();
	auto& device = graphics->getDevice();

	auto pBackend = (ImGui_ImplVulkan_Data*)ImGui::GetIO().BackendRendererUserData;
	pDescriptorSet = std::make_unique<CDescriptorSet>(device.get());
	pDescriptorSet->create(vk::PipelineBindPoint::eGraphics, pBackend->PipelineLayout, EGEditor->getDescriptorPool(), pBackend->DescriptorSetLayout);

	EGEngine->addEventListener(Events::Editor::OpenImageViewer, this, &CEditorImageViewer::OnOpenImage);
}

void CEditorImageViewer::__draw(float fDt)
{
	if (openned_image != invalid_index)
	{
		auto& image = graphics->getImage(openned_image);
		auto& extent = image->getExtent();

		vk::WriteDescriptorSet write{};
		write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		write.dstBinding = 0;
		write.pImageInfo = &image->getDescriptor();
		write.descriptorCount = 1;
		pDescriptorSet->update(write);

		ImGui::Image(pDescriptorSet->get(), ImVec2{ static_cast<float>(extent.width), static_cast<float>(extent.height)});
	}
}

void CEditorImageViewer::OnOpenImage(CEvent& event)
{
	if (openned_image != invalid_index)
		graphics->removeImage(openned_image);

	auto path = event.getParam<std::filesystem::path>(Events::Editor::OpenImageViewer);
	auto name = fs::get_filename(path);
	openned_image = graphics->addImage(name, path);
	bIsOpen = true;
}