#include "MeshLoader.h"

#include "mesh/gltf/GltfLoader.h"

using namespace engine::loaders;
using namespace engine::ecs;

void CMeshLoader::load(FMeshComponent& component)
{
	auto filepath = component.source;
	auto ext = filepath.substr(filepath.find_last_of(".") + 1);

	if (ext == "gltf")
		CGltfLoader::load(component);
}