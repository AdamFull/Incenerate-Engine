#include "MeshLoader.h"

#include "mesh/gltf/GltfLoader.h"
#include "game/SceneGraph.hpp"

#include "system/filesystem/filesystem.h"

using namespace engine::loaders;
using namespace engine::system;
using namespace engine::game;

void CMeshLoader::load(const std::string& source, const entt::entity& root)
{
	auto ext = fs::get_ext(source);

	if (ext == ".gltf")
		std::make_unique<CGltfLoader>()->load(source, root);
}