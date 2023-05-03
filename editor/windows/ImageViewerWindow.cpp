#include "ImageViewerWindow.h"

#include "Editor.h"
#include "Engine.h"

#include "filesystem/vfs_helper.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "graphics/imgui_impl/imgui_impl_vulkan.h"

using namespace engine::graphics;
using namespace engine::filesystem;
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

		static float zoom{ 1.0f };


		auto image_size = ImVec2(static_cast<float>(extent.width), static_cast<float>(extent.height));
		auto screen_size = ImGui::GetContentRegionAvail();

		float scale_factor = std::min(screen_size.x / image_size.x, screen_size.y / image_size.y);

		image_size *= scale_factor;

		//auto availmin = ImGui::GetWindowContentRegionMin();
		//auto avail = ImGui::GetWindowContentRegionMax() - availmin;
		//ImGui::SetCursorPos((avail - image_size) / 2.f + availmin);

		if (ImGui::GetIO().MouseWheel != 0.0f)
		{
			zoom += ImGui::GetIO().MouseWheel * 0.1f;
			zoom = std::max(zoom, 0.1f);
			zoom = std::min(zoom, 10.0f);
		}

		image_size *= zoom;

		ImGui::Image(pDescriptorSet->get(), image_size);
	}
}

void CEditorImageViewer::OnOpenImage(CEvent& event)
{
	if (openned_image != invalid_index)
		graphics->removeImage(openned_image);

	auto path = event.getParam<std::string>(Events::Editor::OpenImageViewer);
	auto name = fs::get_filename(path);
	openned_image = graphics->addImage(name, path);
	bIsOpen = true;
}