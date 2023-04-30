#include "MeshLoader.h"

#include "mesh/gltf/GltfLoader.h"

#include "game/SceneGraph.hpp"

#include "filesystem/vfs_helper.h"

using namespace engine::loaders;
using namespace engine::filesystem;
using namespace engine::ecs;
using namespace engine::game;

void CMeshLoader::load(const std::string& source, const entt::entity& root, FSceneComponent* component)
{
	if (fs::is_gltf_format(source))
		std::make_unique<CGltfLoader>()->load(source, root, component);
}