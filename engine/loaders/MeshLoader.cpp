#include "MeshLoader.h"

#include "mesh/gltf/GltfLoader.h"
#include "game/SceneGraph.hpp"

#include "system/filesystem/filesystem.h"

using namespace engine::loaders;
using namespace engine::system;
using namespace engine::game;

void CMeshLoader::load(const std::filesystem::path& source, const entt::entity& root)
{
	if (fs::is_gltf_format(source))
		std::make_unique<CGltfLoader>()->load(source, root);
}