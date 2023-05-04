#include "EditorProject.h"

#include "filesystem/vfs_helper.h"
#include "Engine.h"
#include "game/SceneSerializer.h"
#include "game/SceneGraph.hpp"

#include "filesystem/native/NativeFileSystem.h"

#include "ecs/components/CameraComponent.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/HierarchyComponent.h"

namespace engine
{
	namespace editor
	{
        void to_json(nlohmann::json& json, const FIncenerateProject& type)
        {
            json = nlohmann::json();
            utl::serialize_from("assets", json, type.assets, !type.assets.empty());
            utl::serialize_from("last_scene", json, type.last_scene, !type.last_scene.empty());
            utl::serialize_from("camera", json, type.camera, true);
        }

        void from_json(const nlohmann::json& json, FIncenerateProject& type)
        {
            utl::parse_to("assets", json, type.assets);
            utl::parse_to("last_scene", json, type.last_scene);
            utl::parse_to("camera", json, type.camera);
        }
	}
}

using namespace engine::game;
using namespace engine::ecs;
using namespace engine::filesystem;
using namespace engine::editor;

CEditorProject::CEditorProject(entt::entity& editor_camera)
{
    this->editor_camera = &editor_camera;
}

bool CEditorProject::make_new(const std::string& path)
{
    auto parent_path = fs::parent_path(path);
    auto assets_path = fs::path_append(parent_path, "assets");
    
    EGFilesystem->mount("/project", std::make_unique<CNativeFileSystem>(parent_path));
    EGFilesystem->createDirectories(fs::path_append("/project", "assets"));

    EGFilesystem->mount("/assets", std::make_unique<CNativeFileSystem>(assets_path));
    
    EGSceneManager->make_new(fs::path_append("/assets", "scene.iescene"));
    EGSceneManager->save();
    
    project.assets = "assets";
    project.last_scene = "scene.iescene";
    if (EGFilesystem->writeBson(fs::path_append("/project", fs::get_filename(path)), project))
    {
        createOrLoadEditorCamera();
        projectpath = path;
        return true;
    }

    return false;
}

bool CEditorProject::open(const std::string& path)
{
    // Mount project filesystem
    EGFilesystem->mount("/project", std::make_unique<CNativeFileSystem>(fs::parent_path(path)));
    projectpath = fs::path_append("/project", fs::get_filename(path));

    if (EGFilesystem->readBson(projectpath, project))
    {
        EGFilesystem->mount("/assets", std::make_unique<CNativeFileSystem>(fs::path_append(fs::parent_path(path), project.assets)));
        EGSceneManager->load(fs::path_append("/assets", project.last_scene));
        createOrLoadEditorCamera();
        return true;
    }

    return false;
}

void CEditorProject::save()
{
    project.camera = CSceneLoader::saveNode(*editor_camera, true);

    if (!projectpath.empty())
        EGFilesystem->writeBson(projectpath, project);

    EGSceneManager->save();
}

void CEditorProject::setScenePath(const std::string& path)
{
    if (!path.empty())
    {
        project.last_scene = path;
        save();
    } 
}

bool CEditorProject::isProjectOpen()
{
    return !projectpath.empty();
}

void CEditorProject::createOrLoadEditorCamera()
{
    auto& registry = EGEngine->getRegistry();

    if (!project.camera.srName.empty())
        *editor_camera = CSceneLoader::loadNode("", project.camera);
    else
    {
        *editor_camera = scenegraph::create_node("editor_camera");
        registry.emplace<FCameraComponent>(*editor_camera, FCameraComponent{});
        auto& camcomp = registry.get<FCameraComponent>(*editor_camera);
        camcomp.sensitivity = 15.f;
        camcomp.active = true;

        auto& transform = registry.get<FTransformComponent>(*editor_camera);
        transform.position = glm::vec3(0.f, 6.f, 24.f);
        transform.rotation = glm::vec3(0.f, -0.1f, -0.9f);
    }

    auto& hierarchy = registry.get<FHierarchyComponent>(*editor_camera);
    hierarchy.hidden = true;

    auto root = EGSceneManager->getRoot();
    scenegraph::attach_child(root, *editor_camera);
}