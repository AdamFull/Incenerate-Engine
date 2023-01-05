#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include "GltfLoader.h"

using namespace engine::loaders;
using namespace engine::ecs;

void CGltfLoader::load(FMeshComponent& component)
{
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF gltfContext;
    std::string error, warning;
}