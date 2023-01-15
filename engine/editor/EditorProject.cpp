#include "EditorProject.h"

#include "system/filesystem/filesystem.h"
#include "Engine.h"

namespace engine
{
	namespace editor
	{
        void to_json(nlohmann::json& json, const FIncenerateProject& type)
        {
            json = nlohmann::json();
            utl::serialize_from("assets", json, type.assets, !type.assets.empty());
            utl::serialize_from("default_scene", json, type.default_scene, !type.default_scene.empty());
        }

        void from_json(const nlohmann::json& json, FIncenerateProject& type)
        {
            utl::parse_to("assets", json, type.assets);
            utl::parse_to("default_scene", json, type.default_scene);
        }
	}
}

using namespace engine::game;
using namespace engine::system;
using namespace engine::editor;

bool CEditorProject::make_new(const std::filesystem::path& path)
{
    FIncenerateProject nproj;
    nproj.assets = "assets";
    nproj.default_scene = "";
    if (fs::write_json(path, nproj, -1, true))
    {
        project = nproj;
        projectpath = path;
        return true;
    }

    return false;
}

bool CEditorProject::open(const std::filesystem::path& path)
{
    if (fs::read_json(path, project))
    {
        fs::set_workdir(path.parent_path() / project.assets);
        EGSceneManager->load(project.default_scene);
        projectpath = path;
        return true;
    }

    return false;
}

void CEditorProject::save()
{
    if (!projectpath.empty())
        fs::write_json(projectpath, project, -1, true);

    EGSceneManager->save();
}