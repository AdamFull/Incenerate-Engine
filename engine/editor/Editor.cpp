#include "Engine.h"

#include "imgui_impl_sdl.h"
#include "imgui_impl_vulkan.h"

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>
#include <imgui/font/IconsMaterialDesignIcons.h>

#include "EditorThemes.h"

#include "system/filesystem/filesystem.h"

#include "windows/ViewportWindow.h"
#include "windows/HierarchyWindow.h"
#include "windows/InspectorWindow.h"
#include "windows/ContentBrowserWindow.h"

#include "ecs/components/TransformComponent.h"
#include "ecs/components/SpotLightComponent.h"
#include "ecs/components/PointLightComponent.h"
#include "ecs/components/DirectionalLightComponent.h"
#include "ecs/components/AudioComponent.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/components/MeshComponent.h"
#include "ecs/components/SceneComponent.h"
#include "ecs/components/ScriptComponent.h"
#include "ecs/components/SkyboxComponent.h"
#include "ecs/components/SpriteComponent.h"


#include "game/SceneSerializer.h"

using namespace engine;
using namespace engine::game;
using namespace engine::graphics;
using namespace engine::system;
using namespace engine::system::window;
using namespace engine::editor;
using namespace engine::ecs;

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

    // Other icons
    mEditorIcons[icons::trash] = ICON_MDI_DELETE;
    mEditorIcons[icons::play] = ICON_MDI_PLAY;
    mEditorIcons[icons::pause] = ICON_MDI_PAUSE;
    mEditorIcons[icons::stop] = ICON_MDI_STOP;
    mEditorIcons[icons::undo] = ICON_MDI_UNDO;
    mEditorIcons[icons::redo] = ICON_MDI_REDO;
    mEditorIcons[icons::cut] = ICON_MDI_CONTENT_CUT;
    mEditorIcons[icons::copy] = ICON_MDI_CONTENT_COPY;
    mEditorIcons[icons::paste] = ICON_MDI_CONTENT_PASTE;
    mEditorIcons[icons::save] = ICON_MDI_CONTENT_SAVE;
    mEditorIcons[icons::save_all] = ICON_MDI_CONTENT_SAVE_ALL;
    mEditorIcons[icons::close] = ICON_MDI_WINDOW_CLOSE;
    mEditorIcons[icons::plus] = ICON_MDI_PLUS;
    mEditorIcons[icons::node] = ICON_MDI_CUBE_SCAN;

    // File icons
    mEditorIcons[icons::folder] = ICON_MDI_FOLDER;
    mEditorIcons[icons::file] = ICON_MDI_FILE;
    mEditorIcons[icons::image_file] = ICON_MDI_IMAGE;
    mEditorIcons[icons::audio_file] = ICON_MDI_FILE_MUSIC;
    mEditorIcons[icons::script_file] = ICON_MDI_SCRIPT;
    mEditorIcons[icons::mesh_file] = ICON_MDI_CUBE;

    // Component icons
    mEditorIcons[get_class_id<FTransformComponent>()] = ICON_MDI_AXIS; // ICON_MDI_VECTOR_LINE
    mEditorIcons[get_class_id<FSpotLightComponent>()] = ICON_MDI_LIGHTBULB;
    mEditorIcons[get_class_id<FPointLightComponent>()] = ICON_MDI_LIGHTBULB;
    mEditorIcons[get_class_id<FDirectionalLightComponent>()] = ICON_MDI_LIGHTBULB;
    mEditorIcons[get_class_id<FAudioComponent>()] = ICON_MDI_VOLUME_HIGH;
    mEditorIcons[get_class_id<FCameraComponent>()] = ICON_MDI_CAMERA;
    mEditorIcons[get_class_id<FMeshComponent>()] = ICON_MDI_SHAPE;
    mEditorIcons[get_class_id<FSceneComponent>()] = ICON_MDI_SHAPE;
    mEditorIcons[get_class_id<FScriptComponent>()] = ICON_MDI_SCRIPT;
    mEditorIcons[get_class_id<FSkyboxComponent>()] = ICON_MDI_EARTH;
    mEditorIcons[get_class_id<FSpriteComponent>()] = ICON_MDI_IMAGE;

    auto& registry = EGCoordinator;
    camera = scenegraph::create_node("editor_camera");
    registry.emplace<FCameraComponent>(camera, FCameraComponent{});

    auto& transform = registry.get<FTransformComponent>(camera);
    transform.position = glm::vec3(1.f, -1.f, 1.f);
    transform.rotation = glm::vec3(0.01f, 0.01f, 0.01f);
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
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New project", "CTRL+N")) {}
            if (ImGui::MenuItem("Open project", "CTRL+O")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("New scene", "CTRL+SHIFT+N")) {}
            if (ImGui::MenuItem("Open scene", "CTRL+SHIFT+O")) {}
            ImGui::Separator();
            if (ImGui::MenuItem((mEditorIcons[icons::save] + " Save").c_str(), "CTRL+S")) { CSceneLoader::save(EGSceneGraph, "scene.json"); }
            if (ImGui::MenuItem((mEditorIcons[icons::save_all] + " Save all").c_str())) {}
            ImGui::Separator();
            if (ImGui::MenuItem((mEditorIcons[icons::close] + " Exit").c_str(), "CTRL+Q")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem((mEditorIcons[icons::undo] + " Undo").c_str(), "CTRL+Z")) {}
            if (ImGui::MenuItem((mEditorIcons[icons::redo] + " Redo").c_str(), "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem((mEditorIcons[icons::cut] + " Cut").c_str(), "CTRL+X")) {}
            if (ImGui::MenuItem((mEditorIcons[icons::copy] + " Copy").c_str(), "CTRL+C")) {}
            if (ImGui::MenuItem((mEditorIcons[icons::paste] + " Paste").c_str(), "CTRL+V")) {}
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
        if (ImGui::BeginMenu("Style"))
        {
            if (ImGui::MenuItem("Standart")) { Themes::standart(); }
            if (ImGui::MenuItem("Cinder")) { Themes::cinder(); }
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
        overlay->draw(fDt);

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

const std::string& CEditor::getIcon(uint32_t id)
{
    return mEditorIcons[id];
}

void CEditor::baseInitialize()
{   
    // Dimensions
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.DisplaySize = ImVec2(CWindowHandle::iWidth, CWindowHandle::iHeight);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    Themes::cinder();
    auto fontfile = (fs::get_workdir(true) / "font" / FONT_ICON_FILE_NAME_MDI).string();

    static const ImWchar icons_ranges[] = { ICON_MIN_MDI, ICON_MAX_MDI, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphOffset.y = 1.0f;
    icons_config.OversampleH = icons_config.OversampleV = 1;
    icons_config.GlyphMinAdvanceX = 4.0f;
    icons_config.SizePixels = 12.0f;

    pSmallIcons = io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF(fontfile.c_str(), 13.f, &icons_config, icons_ranges);

    icons_config.GlyphOffset.y = 15.0f;
    icons_config.SizePixels = 48.0f;
    icons_config.GlyphMaxAdvanceX = 48.f;

    pLargeIcons = io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF(fontfile.c_str(), 48.f, &icons_config, icons_ranges);
}