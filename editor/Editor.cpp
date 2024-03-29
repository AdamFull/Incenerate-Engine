#include "Editor.h"

#include "Engine.h"

#include "graphics/imgui_impl/imgui_impl_sdl.h"
#include "graphics/imgui_impl/imgui_impl_vulkan.h"

#include <imgui/imgui.h>
#include <imgui/imgui_notify.h>
#include <imgui/ImGuizmo.h>
#include <imgui/font/IconsMaterialDesignIcons.h>

#include "CustomControls.h"

#include "EditorThemes.h"

#include "filesystem/vfs_helper.h"

#include "windows/ViewportWindow.h"
#include "windows/HierarchyWindow.h"
#include "windows/InspectorWindow.h"
#include "windows/ContentBrowserWindow.h"
#include "windows/ImageViewerWindow.h"
#include "windows/SystemPerfomanceViewWindow.h"
#include "windows/RenderDebugWindow.h"
#include "windows/MaterialEditorWindow.h"
#include "windows/GraphicsSettingsWindow.h"

#include "operations/RemoveEntityOperation.h"

#include "ecs/components/components.h"

#include "game/SceneGraph.hpp"
#include "game/SceneSerializer.h"

#include "graphics/window/SDL2WindowAdapter.h"

#include "filesystem/native/NativeFileSystem.h"

#include <SessionStorage.hpp>

constexpr const char* editor_local_data = "/editor/editorloc";

namespace engine
{
    namespace editor
    {
        void to_json(nlohmann::json& json, const FRecentProjects& type)
        {
            utl::serialize_from("last", json, type.last, !type.last.empty());
            utl::serialize_from("recent", json, type.recent, !type.recent.empty());
        }

        void from_json(const nlohmann::json& json, FRecentProjects& type)
        {
            utl::parse_to("last", json, type.last);
            utl::parse_to("recent", json, type.recent);
        }
    }
}

using namespace engine;
using namespace engine::game;
using namespace engine::graphics;
using namespace engine::filesystem;
using namespace engine::editor;
using namespace engine::ecs;

CEditor::~CEditor()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    graphics->getDevice()->destroy(&descriptorPool);
    ImGui::DestroyContext();
}

void CEditor::create()
{
    graphics = EGEngine->getGraphics().get();

    auto& device = graphics->getDevice();
    auto& fb = graphics->getFramebuffer("present");

    EGEngine->addEventListener(Events::Graphics::AllFramesDone, this, &CEditor::onAllFramesDone);
    EGEngine->addEventListener(Events::Input::Key, this, &CEditor::onKeyDown);

    pEditorProject = std::make_unique<CEditorProject>(camera);
    pActionBuffer = std::make_unique<CEditorActionBuffer>();

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

    auto& window = graphics->getWindow();

    int32_t wwidth{ 0 }, wheight{ 0 };
    window->getWindowSize(&wwidth, &wheight);

    // Dimensions
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.DisplaySize = ImVec2(static_cast<float>(wwidth), static_cast<float>(wheight));
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    Themes::cinder();

    // Load fonts
    std::vector<uint8_t> fontdata;
    EGFilesystem->readFile(fs::path_append("/embed/font", FONT_ICON_FILE_NAME_MDI), fontdata);

    static const ImWchar icons_ranges[] = { ICON_MIN_MDI, ICON_MAX_MDI, 0 };

    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphOffset.y = 1.0f;
    icons_config.OversampleH = icons_config.OversampleV = 1;
    icons_config.GlyphMinAdvanceX = 4.0f;
    icons_config.SizePixels = 12.0f;

    pSmallIcons = io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromMemoryTTF(fontdata.data(), fontdata.size(), 13.f, &icons_config, icons_ranges);

    icons_config.GlyphOffset.y = 15.0f;
    icons_config.SizePixels = 48.0f;
    icons_config.GlyphMaxAdvanceX = 48.f;

    pLargeIcons = io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromMemoryTTF(fontdata.data(), fontdata.size(), 48.f, &icons_config, icons_ranges);

    // Register windows
    vEditorWindows.emplace_back(std::make_unique<CEditorViewport>("Viewport"));
    vEditorWindows.emplace_back(std::make_unique<CEditorHierarchy>("Hierarchy"));
    vEditorWindows.emplace_back(std::make_unique<CEditorInspector>("Inspector"));
    vEditorWindows.emplace_back(std::make_unique<CEditorContentBrowser>("Content browser"));
    vEditorWindows.emplace_back(std::make_unique<CEditorImageViewer>("Image viewer"));
    vEditorWindows.emplace_back(std::make_unique<CEditorPerfomanceView>("System perfomance view"));
    vEditorWindows.emplace_back(std::make_unique<CRenderDebugWindow>("Render Debug Window"));
    vEditorWindows.emplace_back(std::make_unique<CMaterialEditorWindow>("Material editor"));
    vEditorWindows.emplace_back(std::make_unique<CGraphicsSettingsWindow>("Graphics settings"));

    ImGui_ImplSDL2_InitForVulkan(static_cast<SDL_Window*>(window->getHandle()));
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = device->getVkInstance();
    init_info.PhysicalDevice = device->getPhysical();
    init_info.Device = device->getLogical();
    init_info.QueueFamily = device->getQueueFamilyIndex(family::graphics);
    init_info.Queue = device->getGraphicsQueue();

    // pipeline cache is a potential future optimization, ignoring for now
    init_info.PipelineCache = device->getPipelineCache();
    init_info.DescriptorPool = descriptorPool;
    init_info.Allocator = VK_NULL_HANDLE;
    init_info.MinImageCount = device->getFramesInFlight();
    init_info.ImageCount = device->getFramesInFlight();
    init_info.MSAASamples = vk::SampleCountFlagBits::e1;
    init_info.Subpass = 0;
    init_info.vmaAllocator = device->getVMAAllocator();
    //init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info, fb->getRenderPass());

    auto cmdBuf = CCommandBuffer(device.get());
    cmdBuf.create(true, vk::QueueFlagBits::eGraphics);
    auto commandBuffer = cmdBuf.getCommandBuffer();
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    cmdBuf.submitIdle();
    ImGui_ImplVulkan_DestroyFontUploadObjects();

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
    mEditorIcons[icons::node] = ICON_MDI_GRAPH;

    // File icons
    mEditorIcons[icons::folder] = ICON_MDI_FOLDER;
    mEditorIcons[icons::file] = ICON_MDI_FILE;
    mEditorIcons[icons::image_file] = ICON_MDI_IMAGE;
    mEditorIcons[icons::audio_file] = ICON_MDI_FILE_MUSIC;
    mEditorIcons[icons::script_file] = ICON_MDI_SCRIPT;
    mEditorIcons[icons::mesh_file] = ICON_MDI_CUBE;
    mEditorIcons[icons::add_folder] = ICON_MDI_FOLDER_PLUS;
    mEditorIcons[icons::add_file] = ICON_MDI_FILE_PLUS;
    mEditorIcons[icons::update] = ICON_MDI_UPDATE;

    // Component icons
    mEditorIcons[get_class_id<entt::entity>()] = ICON_MDI_GRAPH;
    mEditorIcons[get_class_id<FTransformComponent>()] = ICON_MDI_AXIS; // ICON_MDI_VECTOR_LINE
    mEditorIcons[get_class_id<FSpotLightComponent>()] = ICON_MDI_HEAD_LIGHTBULB;
    mEditorIcons[get_class_id<FPointLightComponent>()] = ICON_MDI_LIGHTBULB;
    mEditorIcons[get_class_id<FDirectionalLightComponent>()] = ICON_MDI_BRIGHTNESS_7;
    mEditorIcons[get_class_id<FAudioComponent>()] = ICON_MDI_VOLUME_HIGH;
    mEditorIcons[get_class_id<FCameraComponent>()] = ICON_MDI_CAMERA;
    mEditorIcons[get_class_id<FMeshComponent>()] = ICON_MDI_SHAPE;
    mEditorIcons[get_class_id<FSceneComponent>()] = ICON_MDI_SHAPE;
    mEditorIcons[get_class_id<FRigidBodyComponent>()] = ICON_MDI_SHAPE;
    mEditorIcons[get_class_id<FScriptComponent>()] = ICON_MDI_SCRIPT;
    mEditorIcons[get_class_id<FEnvironmentComponent>()] = ICON_MDI_EARTH;
    mEditorIcons[get_class_id<FSpriteComponent>()] = ICON_MDI_IMAGE;
    mEditorIcons[get_class_id<FParticleComponent>()] = ICON_MDI_CREATION;
    mEditorIcons[get_class_id<FTerrainComponent>()] = ICON_MDI_CREATION;

    auto editor_tmp_path = std::filesystem::temp_directory_path() / "incenerate-engine" / "editor";
    if (!std::filesystem::exists(editor_tmp_path))
        std::filesystem::create_directories(editor_tmp_path);

    EGFilesystem->mount("/editor", std::make_unique<CNativeFileSystem>(editor_tmp_path.string()));

    load_editor();

    if (!recproj.recent.empty())
    {
        auto path = std::string(recproj.last);
        pEditorProject->open(path);
        activateEditorCamera();
    }

    for (auto& overlay : vEditorWindows)
        overlay->create();

    ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Engine loading success!" });
}

void CEditor::newFrame(float fDt)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    auto window = static_cast<CSDL2WindowAdapter*>(graphics->getWindow().get());

    auto& events = window->getWinEvents();
    for (auto& sevent : events)
        ImGui_ImplSDL2_ProcessEvent(&sevent);

    //ImGui::ShowDemoWindow();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New project", "Ctrl-N")) { bNeedNewProject = true; }
            if (ImGui::MenuItem("Open project", "Ctrl+O")) { bNeedOpenProject = true; }
            if (ImGui::BeginMenu("Recent projects", !recproj.recent.empty()))
            {
                for (auto& proj : recproj.recent)
                {
                    if (proj != recproj.last)
                    {
                        auto proj_name = fs::get_filename(proj);
                        if (ImGui::MenuItem(proj_name.c_str()))
                            openExistingProject(proj);
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("New scene", "Ctrl-Shift-N", false, pEditorProject->isProjectOpen())) { bNeedNewScene = true; }
            if (ImGui::MenuItem("Open scene", "Ctrl-Shift-O", false, pEditorProject->isProjectOpen())) { bNeedOpenScene = true; }
            ImGui::Separator();
            if (ImGui::MenuItem((mEditorIcons[icons::save] + " Save").c_str(), "Ctrl-S")) { bNeedSave = true; }
            if (ImGui::MenuItem((mEditorIcons[icons::save_all] + " Save all").c_str(), "Ctrl-Shift-S")) { bNeedSaveAll = true; }
            ImGui::Separator();
            if (ImGui::MenuItem((mEditorIcons[icons::close] + " Exit").c_str(), "Ctrl-Q")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem((mEditorIcons[icons::undo] + " Undo").c_str(), "Ctrl-Z", false, pActionBuffer->canUndo())) { bNeedUndo = true; }
            if (ImGui::MenuItem((mEditorIcons[icons::redo] + " Redo").c_str(), "Ctrl-Y", false, pActionBuffer->canRedo())) { bNeedRedo = true; }
            ImGui::Separator();
            if (ImGui::MenuItem((mEditorIcons[icons::cut] + " Cut").c_str(), "Ctrl-X")) {}
            if (ImGui::MenuItem((mEditorIcons[icons::copy] + " Copy").c_str(), "Ctrl-C")) {}
            if (ImGui::MenuItem((mEditorIcons[icons::paste] + " Paste").c_str(), "Ctrl-V")) {}
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
            if (ImGui::MenuItem("Cinder")) { Themes::cinder(); }
            if (ImGui::MenuItem("Darkblue")) { Themes::darkblue(); }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Debug"))
        {
            auto display_normals = CSessionStorage::getInstance()->get<bool>("display_normals");
            if (ImGui::MenuItem("Display normals", "", &display_normals))
                CSessionStorage::getInstance()->set("display_normals", display_normals);

            auto display_physics = CSessionStorage::getInstance()->get<bool>("display_physics");
            if (ImGui::MenuItem("Display physics", "", &display_physics))
                CSessionStorage::getInstance()->set("display_physics", display_physics);

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // File dialogs
    constexpr const char* project_filter[] = { "*.incenerateproj" };
    if (bNeedNewProject)
    {
        static char name_buf[512];
        if (ImGui::FileSave(name_buf, 512, "New incenerate project", 1, project_filter))
            vEditorActions.emplace_back(utl::function<void()>([this, path = std::string(name_buf)]() { makeNewProject(path); }));
        bNeedNewProject = false;
    }

    if (bNeedOpenProject)
    {
        static char name_buf[512];
        if (ImGui::FileOpen(name_buf, 512, "Open project", 1, project_filter))
            vEditorActions.emplace_back(utl::function<void()>([this, path = std::string(name_buf)]() { openExistingProject(path); }));           
        bNeedOpenProject = false;
    }

    constexpr const char* scene_filter[] = { "*.iescene" };
    if (bNeedNewScene)
    {
        static char name_buf[512];
        if (ImGui::FileSave(name_buf, 512, "New scene", 1, scene_filter))
            vEditorActions.emplace_back(utl::function<void()>([this, path = std::string(name_buf)]() { makeNewScene(path); }));
        bNeedNewScene = false;
    }

    if(bNeedOpenScene)
    {
        static char name_buf[512];
        if (ImGui::FileOpen(name_buf, 512, "Open scene", 1, scene_filter))
            vEditorActions.emplace_back(utl::function<void()>([this, path = std::string(name_buf)]() { openExistingScene(path); }));
            
        bNeedOpenScene = false;
    }

    if (bNeedUndo)
    {
        if (pActionBuffer->canUndo())
        {
            ImGui::InsertNotification({ ImGuiToastType_Info, 3000, "Undo action." });
            pActionBuffer->undo();
        }
            
        bNeedUndo = false;
    }

    if (bNeedRedo)
    {
        if (pActionBuffer->canRedo())
        {
            ImGui::InsertNotification({ ImGuiToastType_Info, 3000, "Redo action." });
            pActionBuffer->redo();
        }
            
        bNeedRedo = false;
    }

    if (bNeedSave)
    {
        EGSceneManager->save();
        save_editor();
        ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Scene saved!" });
        bNeedSave = false;
    }

    if (bNeedSaveAll)
    {
        pEditorProject->save();
        ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Project saved!" });
        bNeedSaveAll = false;
    }

    ImGui::DockSpaceOverViewport();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f); // Round borders
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f)); // Background color
    ImGui::RenderNotifications(); // <-- Here we render all notifications
    ImGui::PopStyleVar(1); // Don't forget to Pop()
    ImGui::PopStyleColor(1);

    //ImGui::ShowDemoWindow();
    for (auto& overlay : vEditorWindows)
        overlay->draw(fDt);

    auto commandBuffer = graphics->getCommandBuffer();
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

void CEditor::onAllFramesDone(const std::unique_ptr<IEvent>& event)
{
    for (auto& action : vEditorActions)
        action();
    vEditorActions.clear();
}

void CEditor::makeNewProject(const std::string& project_path)
{
    if (pEditorProject->make_new(project_path))
    {
        selected = entt::null;
        recproj.last = project_path;
        recproj.recent.emplace(recproj.last);
    
        save_editor();
        ImGui::CloseCurrentPopup();
        EGEngine->sendEvent(Events::Editor::ProjectUpdated);
        ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Created new project: %s", recproj.last.c_str()});
        return;
    }
    ImGui::InsertNotification({ ImGuiToastType_Error, 3000, "Failed to create project!" });
}

void CEditor::openExistingProject(const std::string& project_path)
{
    if (pEditorProject->open(project_path))
    {
        selected = entt::null;
        recproj.last = project_path;
        recproj.recent.emplace(recproj.last);
    
        save_editor();
        ImGui::CloseCurrentPopup();
        EGEngine->sendEvent(Events::Editor::ProjectUpdated);
        ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Loaded project: %s.", recproj.last.c_str()});
        return;
    }
    ImGui::InsertNotification({ ImGuiToastType_Error, 3000, "Failed to load project!" });
}

void CEditor::makeNewScene(const std::string& project_path)
{
    if (EGSceneManager->make_new(project_path))
    {
        selected = entt::null;
        pEditorProject->setScenePath(project_path);
        ImGui::CloseCurrentPopup();
        ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Created new scene: %s.", project_path.c_str() });
        return;
    }
    ImGui::InsertNotification({ ImGuiToastType_Error, 3000, "Failed to create new scene!" });
}

void CEditor::openExistingScene(const std::string& project_path)
{
    if (EGSceneManager->load(project_path))
    {
        selected = entt::null;
        pEditorProject->setScenePath(project_path);
        ImGui::CloseCurrentPopup();
        ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Loaded scene: %s.", project_path.c_str() });
        return;
    }
    ImGui::InsertNotification({ ImGuiToastType_Error, 3000, "Failed to load scene!" });
}

void CEditor::selectObject(const entt::entity& object)
{
    auto& registry = EGEngine->getRegistry();
    if(registry.valid(object))
	    selected = object;
}

void CEditor::deselectObject()
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

void CEditor::captureSceneState()
{
    auto& registry = EGEngine->getRegistry();

    mCaptureStates.clear();

    auto view = registry.view<FTransformComponent>();
    for (auto [entity, transform] : view.each())
    {
        mCaptureStates[entity] = {};
        CSceneLoader::saveComponents(mCaptureStates[entity], entity);
    }
}

void CEditor::restoreSceneState()
{
    auto& registry = EGEngine->getRegistry();
    auto view = registry.view<FTransformComponent>();
    for (auto [entity, transform] : view.each())
        CSceneLoader::applyComponents(mCaptureStates[entity], entity);
}

void CEditor::activateEditorCamera()
{
    auto& registry = EGEngine->getRegistry();
    auto view = registry.view<FCameraComponent>();
    for (auto [entity, camera] : view.each()) { camera.active = false; }

    auto& editor_camera = registry.get<FCameraComponent>(camera);
    editor_camera.active = true;
}

void CEditor::activateSceneCamera()
{
    auto& registry = EGEngine->getRegistry();
    auto view = registry.view<FCameraComponent>();
    for (auto [entity, camera] : view.each()) { camera.active = true; }

    auto& editor_camera = registry.get<FCameraComponent>(camera);
    editor_camera.active = false;
}

vk::DescriptorPool& CEditor::getDescriptorPool()
{
	return descriptorPool;
}

const std::string& CEditor::getIcon(uint32_t id)
{
    return mEditorIcons[id];
}

void CEditor::onKeyDown(const std::unique_ptr<IEvent>& event)
{
    auto keys = event->getParam<CKeyDecoder>(Events::Input::Key);
    
    //if (ImGui::MenuItem((mEditorIcons[icons::cut] + " Cut").c_str(), "Ctrl-X")) {}
    //if (ImGui::MenuItem((mEditorIcons[icons::copy] + " Copy").c_str(), "Ctrl-C")) {}
    //if (ImGui::MenuItem((mEditorIcons[icons::paste] + " Paste").c_str(), "Ctrl-V")) {}
    
    if (keys.test(EKeyCode::eKeyLCtrl))
    {
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
        {
            if (ImGui::IsKeyReleased(ImGuiKey_N))
                bNeedNewScene = true;
            else if (ImGui::IsKeyReleased(ImGuiKey_O))
                bNeedOpenScene = true;
            else if (ImGui::IsKeyReleased(ImGuiKey_S))
                bNeedSaveAll = true;
        }
        else if (ImGui::IsKeyReleased(ImGuiKey_Z))
            bNeedUndo = true;
        else if (ImGui::IsKeyReleased(ImGuiKey_Y))
            bNeedRedo = true;
        else if (ImGui::IsKeyReleased(ImGuiKey_N))
            bNeedNewProject = true;
        else if (ImGui::IsKeyReleased(ImGuiKey_O))
            bNeedOpenProject = true;
        else if (ImGui::IsKeyReleased(ImGuiKey_S))
            bNeedSave = true;
    }

    if (ImGui::IsKeyReleased(ImGuiKey_Delete))
    {
        if (selected != entt::null)
            pActionBuffer->addOperation(std::make_unique<CRemoveEntityOperation>(selected));
    }

    if (ImGui::IsKeyReleased(ImGuiKey_PrintScreen))
    {
        auto& device = graphics->getDevice();
        device->takeScreenshot("temp/screenshot.png");
    }
}

void CEditor::load_editor()
{
    if (EGFilesystem->exists(editor_local_data))
        EGFilesystem->readBson(editor_local_data, recproj);
}

void CEditor::save_editor()
{
    EGFilesystem->writeBson(editor_local_data, recproj);
}