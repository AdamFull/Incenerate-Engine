#include "EditorProject.h"

#include "system/filesystem/filesystem.h"
#include "Engine.h"
#include "game/SceneSerializer.h"
#include "game/SceneGraph.hpp"

#include "ecs/components/CameraComponent.h"
#include "ecs/components/TransformComponent.h"

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
using namespace engine::system;
using namespace engine::editor;

CEditorProject::CEditorProject(entt::entity& editor_camera)
{
    this->editor_camera = &editor_camera;
}

bool CEditorProject::make_new(const std::filesystem::path& path)
{
    auto parent_path = path.parent_path();
    auto assets_path = parent_path / "assets";
    fs::set_workdir(assets_path);
    std::filesystem::create_directories(assets_path);

    EGSceneManager->make_new(assets_path / "scene.iescene");
    EGSceneManager->save();

    project.assets = "assets";
    project.last_scene = "scene.iescene";
    if (fs::write_bson(path, project, true))
    {
        createOrLoadEditorCamera();
        projectpath = path;
        return true;
    }

    return false;
}

bool CEditorProject::open(const std::filesystem::path& path)
{
    if (fs::read_bson(path, project))
    {
        fs::set_workdir(path.parent_path() / project.assets);
        createOrLoadEditorCamera();
        EGSceneManager->load(project.last_scene);
        projectpath = path;
        return true;
    }

    return false;
}

void CEditorProject::save()
{
    project.camera = CSceneLoader::saveNode(*editor_camera);

    if (!projectpath.empty())
        fs::write_bson(projectpath, project, true);

    EGSceneManager->save();
}

void CEditorProject::createOrLoadEditorCamera()
{
    if (!project.camera.srName.empty())
        *editor_camera = CSceneLoader::loadNode(project.camera);
    else
    {
        auto& registry = EGCoordinator;
        *editor_camera = scenegraph::create_node("editor_camera");
        registry.emplace<FCameraComponent>(*editor_camera, FCameraComponent{});
        auto& camcomp = registry.get<FCameraComponent>(*editor_camera);
        camcomp.sensitivity = 15.f;

        auto& transform = registry.get<FTransformComponent>(*editor_camera);
        transform.position = glm::vec3(0.f, 8.f, 10.f);
        transform.rotation = glm::vec3(-0.02f, -0.09f, 1.00f);
    }
}