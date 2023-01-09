#include "MeshLoader.h"

#include "mesh/gltf/GltfLoader.h"
#include "game/SceneGraph.hpp"

using namespace engine::loaders;
using namespace engine::game;

void CMeshLoader::load(const std::string& source, const entt::entity& root)
{
	auto ext = source.substr(source.find_last_of(".") + 1);

	if (ext == "gltf")
		std::make_unique<CGltfLoader>()->load(source, root);
}