#include "MeshLoader.h"

#include "mesh/gltf/GltfLoader.h"

using namespace engine::loaders;
using namespace engine::ecs;

void CMeshLoader::load(const std::string& source, const std::unique_ptr<game::CSceneNode>& pRoot)
{
	auto ext = source.substr(source.find_last_of(".") + 1);

	if (ext == "gltf")
		std::make_unique<CGltfLoader>()->load(source, pRoot);
}