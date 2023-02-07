#include "MeshLoader.h"

#include "mesh/gltf/GltfLoader.h"
#include "mesh/obj/ObjLoader.h"

#include "game/SceneGraph.hpp"

#include "system/filesystem/filesystem.h"

using namespace engine::loaders;
using namespace engine::system;
using namespace engine::ecs;
using namespace engine::game;

void CMeshLoader::load(const std::filesystem::path& source, const entt::entity& root, FSceneComponent* component)
{
	if (fs::is_gltf_format(source))
		std::make_unique<CGltfLoader>()->load(source, root, component);
	else if (fs::is_obj_format(source))
		std::make_unique<CObjLoader>()->load(source, root, component);
	else if (fs::is_fbx_format(source));
}