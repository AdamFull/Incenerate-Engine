#include "Engine.h"

#include "imgui_impl_sdl.h"
#include "imgui_impl_vulkan.h"

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>
#include <imgui/IconsFontAwesome6.h>

#include "system/filesystem/filesystem.h"

#include "windows/ViewportWindow.h"
#include "windows/HierarchyWindow.h"
#include "windows/InspectorWindow.h"
#include "windows/ContentBrowserWindow.h"

#include "game/SceneSerializer.h"

using namespace engine;
using namespace engine::game;
using namespace engine::graphics;
using namespace engine::system;
using namespace engine::system::window;
using namespace engine::editor;

CEditor::~CEditor()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    EGGraphics->getDevice()->destroy(&descriptorPool);
    ImGui::DestroyContext();
}

void CEditor::create()
{
    auto& device = EGGraphics->getDevice();
    auto& fb = EGGraphics->getFramebuffer("present");

    vk::DescriptorPoolSize pool_sizes[] =
    {
        {vk::DescriptorType::eSampler, 1000},
        {vk::DescriptorType::eCombinedImageSampler, 1000},
        {vk::DescriptorType::eSampledImage, 1000},
        {vk::DescriptorType::eStorageImage, 1000},
        {vk::DescriptorType::eUniformTexelBuffer, 1000},
        {vk::DescriptorType::eStorageTexelBuffer, 1000},
        {vk::DescriptorType::eUniformBuffer, 1000},
        {vk::DescriptorType::eStorageBuffer, 1000},
        {vk::DescriptorType::eUniformBufferDynamic, 1000},
        {vk::DescriptorType::eStorageBufferDynamic, 1000},
        {vk::DescriptorType::eInputAttachment, 1000}
    };

    vk::DescriptorPoolCreateInfo pool_info = {};
    pool_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    vk::Result res = device->create(pool_info, &descriptorPool);
    assert(res == vk::Result::eSuccess && "Cannot create descriptor pool.");

    ImGui::CreateContext();
    baseInitialize();

    // Register windows
    vEditorWindows.emplace_back(std::make_unique<CEditorViewport>("Viewport"));
    vEditorWindows.emplace_back(std::make_unique<CEditorHierarchy>("Hierarchy"));
    vEditorWindows.emplace_back(std::make_unique<CEditorInspector>("Inspector"));
    vEditorWindows.emplace_back(std::make_unique<CEditorContentBrowser>("Content browser"));

    ImGui_ImplSDL2_InitForVulkan(EGWindow->getWindowPointer());
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = device->getVkInstance();
    init_info.PhysicalDevice = device->getPhysical();
    init_info.Device = device->getLogical();
    init_info.QueueFamily = device->findQueueFamilies().graphicsFamily.value();
    init_info.Queue = device->getGraphicsQueue();

    // pipeline cache is a potential future optimization, ignoring for now
    init_info.PipelineCache = device->getPipelineCache();
    init_info.DescriptorPool = descriptorPool;
    init_info.Allocator = VK_NULL_HANDLE;
    init_info.MinImageCount = device->getFramesInFlight();
    init_info.ImageCount = device->getFramesInFlight();
    init_info.MSAASamples = vk::SampleCountFlagBits::e1;
    init_info.Subpass = 0;
    //init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info, fb->getRenderPass());

    auto cmdBuf = CCommandBuffer(device.get());
    cmdBuf.create(true, vk::QueueFlagBits::eTransfer);
    auto commandBuffer = cmdBuf.getCommandBuffer();
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    cmdBuf.submitIdle();
    ImGui_ImplVulkan_DestroyFontUploadObjects();

    for (auto& overlay : vEditorWindows)
       overlay->create();
}

void CEditor::newFrame(float fDt)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    auto& events = EGWindow->getWinEvents();
    for (auto& sevent : events)
        ImGui_ImplSDL2_ProcessEvent(&sevent);

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(ICON_FA_FILE " File"))
        {
            if (ImGui::MenuItem(ICON_FA_FILE_PEN " New scene", "CTRL+N")) {}
            if (ImGui::MenuItem("Open scene", "CTRL+O")) {}
            if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK" Save current", "CTRL+S")) { CSceneLoader::save(EGSceneGraph, "scene.json"); }
            if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK" Save current as...", "CTRL+ALT+S")) {}
            if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK" Save all")) {}
            ImGui::Separator();
            if (ImGui::MenuItem(ICON_FA_POWER_OFF " Exit", "CTRL+Q")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem(ICON_FA_ARROW_RIGHT" Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window"))
        {
            for (auto& overlay : vEditorWindows)
            {
                if (ImGui::MenuItem(overlay->getName().c_str(), nullptr, overlay->isOpen()))
                    overlay->toggleEnable();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::DockSpaceOverViewport();

    //ImGui::ShowDemoWindow();
    for (auto& overlay : vEditorWindows)
        overlay->draw();

    auto commandBuffer = EGGraphics->getCommandBuffer();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Render();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
    ImDrawData* drawdata = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(drawdata, commandBuffer);
}

void CEditor::selectObject(const entt::entity& object)
{
	selected = object;
}

void CEditor::deselectObject(const entt::entity& object)
{
	selected = entt::null;
}

void CEditor::deselectAll()
{
	selected = entt::null;
}

bool CEditor::isSelected(const entt::entity& object) const
{
	return selected == object;
}

const entt::entity& CEditor::getLastSelection() const
{
	return selected;
}

vk::DescriptorPool& CEditor::getDescriptorPool()
{
	return descriptorPool;
}

void CEditor::baseInitialize()
{
    // Color scheme
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 5.3f;
    style.PopupRounding = style.TabRounding = style.GrabRounding = style.FrameRounding = 2.3f;
    style.ScrollbarRounding = 5.0f;
    style.FrameBorderSize = 1.0f;
    style.ItemSpacing.y = 6.5f;

    style.Colors[ImGuiCol_Text] = { 0.73333335f, 0.73333335f, 0.73333335f, 1.00f };
    style.Colors[ImGuiCol_TextDisabled] = { 0.34509805f, 0.34509805f, 0.34509805f, 1.00f };
    style.Colors[ImGuiCol_WindowBg] = { 0.23529413f, 0.24705884f, 0.25490198f, 0.94f };
    style.Colors[ImGuiCol_ChildBg] = { 0.23529413f, 0.24705884f, 0.25490198f, 0.00f };
    style.Colors[ImGuiCol_PopupBg] = { 0.23529413f, 0.24705884f, 0.25490198f, 0.94f };
    style.Colors[ImGuiCol_Border] = { 0.33333334f, 0.33333334f, 0.33333334f, 0.50f };
    style.Colors[ImGuiCol_BorderShadow] = { 0.15686275f, 0.15686275f, 0.15686275f, 0.00f };
    style.Colors[ImGuiCol_FrameBg] = { 0.16862746f, 0.16862746f, 0.16862746f, 0.54f };
    style.Colors[ImGuiCol_FrameBgHovered] = { 0.453125f, 0.67578125f, 0.99609375f, 0.67f };
    style.Colors[ImGuiCol_FrameBgActive] = { 0.47058827f, 0.47058827f, 0.47058827f, 0.67f };
    style.Colors[ImGuiCol_TitleBg] = { 0.04f, 0.04f, 0.04f, 1.00f };
    style.Colors[ImGuiCol_TitleBgCollapsed] = { 0.16f, 0.29f, 0.48f, 1.00f };
    style.Colors[ImGuiCol_TitleBgActive] = { 0.00f, 0.00f, 0.00f, 0.51f };
    style.Colors[ImGuiCol_MenuBarBg] = { 0.27058825f, 0.28627452f, 0.2901961f, 0.80f };
    style.Colors[ImGuiCol_ScrollbarBg] = { 0.27058825f, 0.28627452f, 0.2901961f, 0.60f };
    style.Colors[ImGuiCol_ScrollbarGrab] = { 0.21960786f, 0.30980393f, 0.41960788f, 0.51f };
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = { 0.21960786f, 0.30980393f, 0.41960788f, 1.00f };
    style.Colors[ImGuiCol_ScrollbarGrabActive] = { 0.13725491f, 0.19215688f, 0.2627451f, 0.91f };
    // style->Colors[ImGuiCol_ComboBg]               = {0.1f, 0.1f, 0.1f, 0.99f};
    style.Colors[ImGuiCol_CheckMark] = { 0.90f, 0.90f, 0.90f, 0.83f };
    style.Colors[ImGuiCol_SliderGrab] = { 0.70f, 0.70f, 0.70f, 0.62f };
    style.Colors[ImGuiCol_SliderGrabActive] = { 0.30f, 0.30f, 0.30f, 0.84f };
    style.Colors[ImGuiCol_Button] = { 0.33333334f, 0.3529412f, 0.36078432f, 0.49f };
    style.Colors[ImGuiCol_ButtonHovered] = { 0.21960786f, 0.30980393f, 0.41960788f, 1.00f };
    style.Colors[ImGuiCol_ButtonActive] = { 0.13725491f, 0.19215688f, 0.2627451f, 1.00f };
    style.Colors[ImGuiCol_Header] = { 0.33333334f, 0.3529412f, 0.36078432f, 0.53f };
    style.Colors[ImGuiCol_HeaderHovered] = { 0.453125f, 0.67578125f, 0.99609375f, 0.67f };
    style.Colors[ImGuiCol_HeaderActive] = { 0.47058827f, 0.47058827f, 0.47058827f, 0.67f };
    style.Colors[ImGuiCol_Separator] = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
    style.Colors[ImGuiCol_SeparatorHovered] = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
    style.Colors[ImGuiCol_SeparatorActive] = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
    style.Colors[ImGuiCol_ResizeGrip] = { 1.00f, 1.00f, 1.00f, 0.85f };
    style.Colors[ImGuiCol_ResizeGripHovered] = { 1.00f, 1.00f, 1.00f, 0.60f };
    style.Colors[ImGuiCol_ResizeGripActive] = { 1.00f, 1.00f, 1.00f, 0.90f };
    style.Colors[ImGuiCol_PlotLines] = { 0.61f, 0.61f, 0.61f, 1.00f };
    style.Colors[ImGuiCol_PlotLinesHovered] = { 1.00f, 0.43f, 0.35f, 1.00f };
    style.Colors[ImGuiCol_PlotHistogram] = { 0.90f, 0.70f, 0.00f, 1.00f };
    style.Colors[ImGuiCol_PlotHistogramHovered] = { 1.00f, 0.60f, 0.00f, 1.00f };
    style.Colors[ImGuiCol_TextSelectedBg] = { 0.18431373f, 0.39607847f, 0.79215693f, 0.90f };

    // Dimensions
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.DisplaySize = ImVec2(CWindowHandle::iWidth, CWindowHandle::iHeight);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    

    auto fontfile = (fs::get_workdir() / "engine" / "font" / FONT_ICON_FILE_NAME_FAR).string();

    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphMaxAdvanceX = 13.f;
    icons_config.GlyphOffset.y = 13.f / 4.f;

    pSmallIcons = io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF(fontfile.c_str(), 13.f, &icons_config, icons_ranges);

    icons_config.GlyphMaxAdvanceX = 48.f;
    icons_config.GlyphOffset.y = 48.f / 4.f;

    pLargeIcons = io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF(fontfile.c_str(), 48.f, &icons_config, icons_ranges);
}