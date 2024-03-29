#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include "GltfLoader.h"
#include "Engine.h"
#include "game/SceneGraph.hpp"

#include "graphics/rendering/material/Material.h"

#include "ecs/components/components.h"

#include "filesystem/vfs_helper.h"

#include "graphics/image/ImageLoader.h"

#include "meshoptimizer/src/meshoptimizer.h"
#include "loaders/mesh/MeshHelper.h"

#include "Helpers.h"

using namespace engine;
using namespace engine::loaders;
using namespace engine::graphics;
using namespace engine::game;
using namespace engine::ecs;
using namespace engine::filesystem;


std::string url_decode(const std::string& str)
{
    std::string ret;
    char ch;
    int i, ii, len = str.length();

    for (i = 0; i < len; i++) 
    {
        if (str[i] != '%') 
        {
            if (str[i] == '+')
                ret += ' ';
            else
                ret += str[i];
        }
        else 
        {
            sscanf(str.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret += ch;
            i = i + 2;
        }
    }
    return ret;
}

int getTextureIndex(const std::string& name, const tinygltf::Value& val)
{
    if (val.Has(name))
    {
        auto obj = val.Get(name);
        if (obj.Has("index"))
        {
            auto idx = obj.Get("index");
            return idx.GetNumberAsInt();
        }
    }

    return -1;
}

double getDoubleValueOrDefault(const std::string& name, const tinygltf::Value& val, double _default = 0.0)
{
    if (val.Has(name))
    {
        auto obj = val.Get(name);
        return obj.GetNumberAsDouble();
    }

    return _default;
}

glm::vec3 getVec3OrDefault(const std::string& name, const tinygltf::Value& val, glm::vec3 _default = glm::vec3(0.f))
{
    if (val.Has(name))
    {
        auto obj = val.Get(name);
        auto x = const_cast<tinygltf::Value*>(&obj.Get(0));
        auto y = const_cast<tinygltf::Value*>(&obj.Get(1));
        auto z = const_cast<tinygltf::Value*>(&obj.Get(2));
        return glm::vec3(x->GetNumberAsDouble(), y->GetNumberAsDouble(), z->GetNumberAsDouble());
    }

    return _default;
}

// Based on https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanglTFModel.cpp
bool CGltfLoader::loadImageDataFuncEmpty(tinygltf::Image* image, const int imageIndex, std::string* err, std::string* warn, int req_width, int req_height, const unsigned char* bytes, int size, void* userData)
{
    auto* loader = static_cast<CGltfLoader*>(userData);

    // Full path
    //auto texture_path = std::filesystem::weakly_canonical(loader->fsParentPath / url_decode(image->uri));

    //std::unique_ptr<FImageCreateInfo> pImageCI;
    //CImageLoader::load(texture_path, pImageCI);
    //
    //image->width = pImageCI->baseWidth;
    //image->height = pImageCI->baseHeight;
    //image->component = pImageCI->dataSize / (pImageCI->baseWidth * pImageCI->baseHeight);

    return true;//tinygltf::LoadImageData(image, imageIndex, error, warning, req_width, req_height, bytes, size, userData);
}

bool gltfFileSystemFileExists(const std::string& filename, void* userdata)
{
    return EGFilesystem->exists(filename);
}

std::string gltfFileSystemExpandFilePath(const std::string& filename, void* userdata)
{
    return fs::normalize(fs::to_posix_path(filename));
}

bool gltfFileSystemReadFile(std::vector<uint8_t>* u8data, std::string* err, const std::string& filename, void* userdata)
{
    return EGFilesystem->readFile(filename, *u8data);
}

bool gltfFileSystemWriteFile(std::string* err, const std::string& filename, const std::vector<uint8_t>& u8data, void* userdata)
{
    return EGFilesystem->writeFile(filename, u8data);
}

void CGltfLoader::load(const const std::string& source, const entt::entity& root, FSceneComponent* component)
{
    auto& graphics = EGEngine->getGraphics();
    head = root;

    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF gltfContext;
    tinygltf::FsCallbacks gltfFsCallbacks;
    gltfFsCallbacks.user_data = this;
    gltfFsCallbacks.FileExists = &gltfFileSystemFileExists;
    gltfFsCallbacks.ExpandFilePath = &gltfFileSystemExpandFilePath;
    gltfFsCallbacks.ReadWholeFile = &gltfFileSystemReadFile;
    gltfFsCallbacks.WriteWholeFile = &gltfFileSystemWriteFile;

    std::string error, warning;
    gltfContext.SetImageLoader(&CGltfLoader::loadImageDataFuncEmpty, this);
    gltfContext.SetFsCallbacks(gltfFsCallbacks);

    fsParentPath = fs::parent_path(source);
    auto ext = fs::get_ext(source);

    bool fileLoaded{ false };
    if (ext == ".glb")
        fileLoaded = gltfContext.LoadBinaryFromFile(&gltfModel, &error, &warning, source);
    else if (ext == ".gltf")
        fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, source);

    if (!warning.empty())
        log_warning("\nWarnings while loading gltf scene \"{}\": \n{}", source, warning);
    if (!error.empty())
        log_error("\nErrors while loading gltf scene \"{}\": \n{}", source, error);

    if (fileLoaded)
    {
        vbo_id = graphics->addVertexBuffer(source);

        loadTextures(gltfModel);
        loadMaterials(gltfModel);

        const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];

        for (auto& node_idx : scene.nodes)
        {
            const tinygltf::Node node = gltfModel.nodes[node_idx];
            loadNode(root, node, node_idx, gltfModel, 1.0);
        }

        FShaderCreateInfo specials;
        specials.pipeline_stage = "deferred";
        specials.vertex_type = EVertexType::eDefault;
        specials.cull_mode = vk::CullModeFlagBits::eBack;
        specials.front_face = vk::FrontFace::eCounterClockwise;
        specials.use_bindles_textures = true;
        specials.depth_test = true;

        for (auto& mat_id : vMaterials)
        {
            auto& material = graphics->getMaterial(mat_id);
            auto& material_params = material->getParameters();

            specials.double_sided = material_params.doubleSided;
            specials.usages = material->getUsageCount();
            specials.alpha_blend = material_params.alphaMode;

            std::stringstream ss{};
            ss << "default:default";
            for (auto& definition : material_params.vCompileDefinitions)
                ss << ":" << definition;

            material->setShader(graphics->addShader(ss.str(), specials));
        }

        if (gltfModel.animations.size() > 0)
            loadAnimations(gltfModel, component);
        loadSkins(gltfModel, component);

        auto& vbo = graphics->getVertexBuffer(vbo_id);
        vbo->addMeshData(vVertexBuffer, vIndexBuffer);
        vbo->setLoaded();
    }
}

void CGltfLoader::loadNode(const entt::entity& parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, float globalscale)
{
    auto& registry = EGEngine->getRegistry();

    auto& phierarchy = registry.get<FHierarchyComponent>(parent);

    auto obj_name = node.name.empty() ? phierarchy.name + "_" + std::to_string(nodeIndex) : node.name;
    auto entity = scenegraph::create_node(obj_name);

    mIndexToEntity[nodeIndex] = entity;

    auto& transform = registry.get<FTransformComponent>(entity);

    // Loading position data
    if (!node.translation.empty())
        transform.position = glm::make_vec3(node.translation.data());
    // Loading rotation data
    if (!node.rotation.empty())
    {
        //TODO: refactor transform for using quaterion
        transform.rotation = glm::make_quat(node.rotation.data());
    }
    // Loading scale data
    if (!node.scale.empty())
        transform.scale = glm::make_vec3(node.scale.data());

    if (!node.matrix.empty())
        transform.matrix = glm::make_mat4(node.matrix.data());

    // Node with children
    if (node.children.size() > 0)
    {
        for (auto i = 0; i < node.children.size(); i++)
            loadNode(entity, model.nodes[node.children[i]], node.children[i], model, globalscale);
    }

    if (node.mesh > -1) loadMeshComponent(entity, node, model);
    if (node.camera > -1) loadCameraComponent(entity, node, model);

    if (!node.extensions.empty())
    {
        auto light_support = node.extensions.find("KHR_lights_punctual");
        if (light_support != node.extensions.end())
        {
            auto& extension = light_support->second;
            auto source = extension.Get("light");
            loadLightComponent(entity, source.GetNumberAsInt(), node, model);
        }
    }

    scenegraph::attach_child(parent, entity);
}

void CGltfLoader::loadMeshComponent(const entt::entity& parent, const tinygltf::Node& node, const tinygltf::Model& model)
{
    auto& graphics = EGEngine->getGraphics();

    auto& registry = EGEngine->getRegistry();
    const tinygltf::Mesh& mesh = model.meshes[node.mesh];

    registry.emplace<FMeshComponent>(parent, FMeshComponent{});
    auto& meshComponent = registry.get<FMeshComponent>(parent);
    meshComponent.skin = node.skin;
    meshComponent.head = head;

    for (size_t j = 0; j < mesh.primitives.size(); j++)
    {
        std::vector<uint32_t> indexBuffer;
        std::vector<FVertex> vertexBuffer;

        const tinygltf::Primitive& primitive = mesh.primitives[j];
        //if (primitive.indices < 0)
        //    continue;

        uint32_t indexStart = static_cast<uint32_t>(vIndexBuffer.size());
        uint32_t vertexStart = static_cast<uint32_t>(vVertexBuffer.size());
        uint32_t indexCount = 0;
        uint32_t vertexCount = 0;
        glm::vec3 posMin{};
        glm::vec3 posMax{};
        bool bHasSkin{ false }, bHasNormals{ false }, bHasTangents{ false };

        // Vertices
        {
            const float* bufferPos = nullptr;
            const float* bufferNormals = nullptr;
            const float* bufferTexCoords = nullptr;
            const float* bufferColors = nullptr;
            const float* bufferTangents = nullptr;
            uint32_t numColorComponents;
            const uint16_t* bufferJoints = nullptr;
            const float* bufferWeights = nullptr;

            // Position attribute is required
            assert(primitive.attributes.find("POSITION") != primitive.attributes.end());
            const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
            const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
            bufferPos = reinterpret_cast<const float*>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
            posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
            posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);

            // Load model normals
            if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
            {
                const tinygltf::Accessor& normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
                const tinygltf::BufferView& normView = model.bufferViews[normAccessor.bufferView];
                bufferNormals = reinterpret_cast<const float*>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
                bHasNormals = true;
            }

            // Load UV coordinates
            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
            {
                const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
                const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
                bufferTexCoords = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
            }

            // Load mesh color data
            if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
            {
                const tinygltf::Accessor& colorAccessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
                const tinygltf::BufferView& colorView = model.bufferViews[colorAccessor.bufferView];
                // Color buffer are either of type vec3 or vec4
                numColorComponents = colorAccessor.type == TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3 ? 3 : 4;
                bufferColors = reinterpret_cast<const float*>(&(model.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset]));
            }

            // Load tangent
            if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
            {
                const tinygltf::Accessor& tangentAccessor = model.accessors[primitive.attributes.find("TANGENT")->second];
                const tinygltf::BufferView& tangentView = model.bufferViews[tangentAccessor.bufferView];
                bufferTangents = reinterpret_cast<const float*>(&(model.buffers[tangentView.buffer].data[tangentAccessor.byteOffset + tangentView.byteOffset]));
                bHasTangents = true;
            }

            // Load joints
            if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end())
            {
                const tinygltf::Accessor& jointAccessor = model.accessors[primitive.attributes.find("JOINTS_0")->second];
                const tinygltf::BufferView& jointView = model.bufferViews[jointAccessor.bufferView];
                bufferJoints = reinterpret_cast<const uint16_t*>(&(model.buffers[jointView.buffer].data[jointAccessor.byteOffset + jointView.byteOffset]));
            }

            // Load weight
            if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end())
            {
                const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("WEIGHTS_0")->second];
                const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
                bufferWeights = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
            }

            bHasSkin = (bufferJoints && bufferWeights);

            vertexCount = static_cast<uint32_t>(posAccessor.count);

            for (size_t v = 0; v < posAccessor.count; v++)
            {
                FVertex vert{};
                vert.pos = glm::vec4(glm::make_vec3(&bufferPos[v * 3]), 1.0f);
                vert.normal = glm::normalize(glm::vec3(bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f)));
                //vert.normal = glm::vec3(0.f);
                vert.texcoord = bufferTexCoords ? glm::make_vec2(&bufferTexCoords[v * 2]) : glm::vec3(0.0f);
                if (bufferColors)
                {
                    switch (numColorComponents)
                    {
                    case 3:
                        vert.color = glm::vec4(glm::make_vec3(&bufferColors[v * 3]), 1.0f);
                        break;
                    case 4:
                        vert.color = glm::make_vec4(&bufferColors[v * 4]);
                        break;
                    }
                }
                else
                {
                    vert.color = glm::vec4(1.0f);
                }
                vert.tangent = bufferTangents ? glm::vec4(glm::make_vec4(&bufferTangents[v * 4])) : glm::vec4(0.0f);
                //vert.tangent = glm::vec4(0.f);
                // TODO: Add skinning
                vert.joint0 = bHasSkin ? glm::vec4(glm::make_vec4(&bufferJoints[v * 4])) : glm::vec4(0.0f);
                vert.weight0 = bHasSkin ? glm::make_vec4(&bufferWeights[v * 4]) : glm::vec4(0.0f);
                vertexBuffer.push_back(vert);
            }
        }

        // Indices
        if(primitive.indices >= 0)
        {
            const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

            indexCount = static_cast<uint32_t>(accessor.count);

            switch (accessor.componentType)
            {
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
            {
                uint32_t* buf = new uint32_t[accessor.count];
                memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
                for (size_t index = 0; index < accessor.count; index++)
                    indexBuffer.push_back(buf[index] + vertexStart);
                delete[] buf;
                break;
            }
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
            {
                uint16_t* buf = new uint16_t[accessor.count];
                memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
                for (size_t index = 0; index < accessor.count; index++)
                    indexBuffer.push_back(buf[index] + vertexStart);
                delete[] buf;
                break;
            }
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
            {
                uint8_t* buf = new uint8_t[accessor.count];
                memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
                for (size_t index = 0; index < accessor.count; index++)
                    indexBuffer.push_back(buf[index] + vertexStart);
                delete[] buf;
                break;
            }
            default:
            {
                std::stringstream ss;
                ss << "Index component type " << accessor.componentType << " not supported!";
                log_warning("Warning while loading gltf scene: {}", ss.str());
                return;
            } break;
            }
        }

        //Creating meshlet
        FMeshlet meshlet;
        meshlet.setDimensions(posMin, posMax);

        // Attaching materials
        if (!vMaterials.empty())
        {
            auto& material = primitive.material > -1 ? vMaterials.at(primitive.material) : vMaterials.back();
            auto& pMaterial = graphics->getMaterial(material);
            pMaterial->incrementUsageCount();
            auto& params = pMaterial->getParameters();
            if (bHasSkin) params.vCompileDefinitions.emplace("HAS_SKIN");
            if (bHasNormals) params.vCompileDefinitions.emplace("HAS_NORMALS");
            if (bHasTangents) params.vCompileDefinitions.emplace("HAS_TANGENTS");
            meshlet.material = material;
        }

        // Optimizing and generating lod
        for (auto& index : indexBuffer)
            index -= vertexStart;
        
        auto* vertices = reinterpret_cast<float*>(vertexBuffer.data());

        // Optimize mesh
        //meshopt_optimizeOverdraw(temp_indexBuffer.data(), temp_indexBuffer.data(), indexCount, vertices, vertexBuffer.size(), sizeof(FVertex), 1.05f);
        meshopt_optimizeVertexFetch(vertices, indexBuffer.data(), indexCount, vertices, vertexBuffer.size(), sizeof(FVertex));
        meshopt_optimizeVertexCache(indexBuffer.data(), indexBuffer.data(), indexBuffer.size(), vertexBuffer.size());

        // Generating lod
        for (uint32_t lod = 0; lod < MAX_LEVEL_OF_DETAIL; ++lod)
        {
            std::vector<uint32_t> indexBufferCpy = indexBuffer;
        
            indexStart = static_cast<uint32_t>(vIndexBuffer.size());
        
            const float lod_quality = 1.f / (1 << lod);
        
            size_t target_index_count = size_t(double(indexBufferCpy.size() / 3) * lod_quality) * 3;
            const float target_error = 1e-2f;
        
            bool lock_borders = true;
            unsigned int options = lock_borders ? meshopt_SimplifyLockBorder : 0;
        
            float lod_error = 0.f;
            indexBufferCpy.resize(meshopt_simplify<uint32_t>(indexBufferCpy.data(), indexBuffer.data(), indexBuffer.size(), reinterpret_cast<float*>(vertexBuffer.data()), vertexBuffer.size(), sizeof(FVertex),
                target_index_count, target_error, options, &lod_error));
        
            indexCount = indexBufferCpy.size();
        
            for (size_t i = 0ull; i < indexBufferCpy.size(); ++i)
                indexBufferCpy[i] += vertexStart;
        
            meshlet.levels_of_detail[lod].begin_index = indexStart;
            meshlet.levels_of_detail[lod].index_count = indexCount;
            meshlet.levels_of_detail[lod].begin_vertex = vertexStart;
            meshlet.levels_of_detail[lod].vertex_count = vertexCount;
            vIndexBuffer.insert(vIndexBuffer.end(), indexBufferCpy.begin(), indexBufferCpy.end());
        }

        meshComponent.vMeshlets.emplace_back(meshlet);

        vVertexBuffer.insert(vVertexBuffer.end(), vertexBuffer.begin(), vertexBuffer.end());
    }

    meshComponent.vbo_id = vbo_id;
    meshComponent.loaded = true;

    std::sort(meshComponent.vMeshlets.begin(), meshComponent.vMeshlets.end(), [](const FMeshlet& a, const FMeshlet& b) { return a.material < b.material; });

    // temp (generate instances)
    //for (int k = 0; k < 4; k++)
    //{
    //    for (int i = 0; i < 10; i++)
    //    {
    //        for (int j = 0; j < 10; j++)
    //        {
    //            auto posX = static_cast<float>(j * 20.f);
    //            auto posY = static_cast<float>(k * 30.f);
    //            auto posZ = static_cast<float>(i * 20.f);
    //            auto translation = glm::vec3{ posX, posY, posZ };
    //
    //            auto& instance = meshComponent.vInstances[meshComponent.instanceCount];
    //            instance.model = glm::mat4(1.f);
    //            instance.model = glm::translate(instance.model, translation);
    //            instance.model = glm::scale(instance.model, glm::vec3{ 1.f });
    //            
    //            instance.color = glm::normalize(glm::vec4{ translation, 1.f});
    //            instance.color.w = 1.f;
    //
    //            meshComponent.instanceCount++;
    //        }
    //    }
    //}     
}

void CGltfLoader::loadCameraComponent(const entt::entity& parent, const tinygltf::Node& node, const tinygltf::Model& model)
{
    auto& registry = EGEngine->getRegistry();
    const tinygltf::Camera camera = model.cameras[node.camera];
    FCameraComponent cameraComponent;

    if (camera.type == "orthographic")
    {
        cameraComponent.type = ECameraType::eOrthographic;
        cameraComponent.xmag = camera.orthographic.xmag;
        cameraComponent.ymag = camera.orthographic.ymag;
        cameraComponent.nearPlane = camera.orthographic.znear;
        cameraComponent.farPlane = camera.orthographic.zfar;
    }
    else if (camera.type == "perspective")
    {
        cameraComponent.type = ECameraType::ePerspective;
        cameraComponent.fieldOfView = camera.perspective.yfov;
        cameraComponent.nearPlane = camera.perspective.znear;
        cameraComponent.farPlane = camera.perspective.zfar;
    }

    registry.emplace<FCameraComponent>(parent, cameraComponent);
}

void CGltfLoader::loadLightComponent(const entt::entity& parent, uint32_t light_index, const tinygltf::Node& node, const tinygltf::Model& model)
{
    auto& registry = EGEngine->getRegistry();
    const tinygltf::Light light = model.lights[light_index];

    glm::vec3 color;

    if (light.color.empty())
        color = glm::vec3(1.f);
    else
        color = glm::make_vec3(light.color.data());

    if (light.type == "directional")
    {
        FDirectionalLightComponent lightComponent;
        lightComponent.color = color;
        lightComponent.intencity = light.intensity;
        registry.emplace<FDirectionalLightComponent>(parent, lightComponent);
    }
    else if (light.type == "spot")
    {
        FSpotLightComponent lightComponent;
        lightComponent.color = color;
        lightComponent.innerAngle = light.spot.innerConeAngle;
        lightComponent.outerAngle = light.spot.outerConeAngle;
        lightComponent.intencity = light.intensity;
        registry.emplace<FSpotLightComponent>(parent, lightComponent);
    }
    else if (light.type == "point")
    {
        FPointLightComponent lightComponent;
        lightComponent.color = color;
        lightComponent.intencity = light.intensity;
        lightComponent.radius = light.range;
        registry.emplace<FPointLightComponent>(parent, lightComponent);
    }
}

void CGltfLoader::loadMaterials(const tinygltf::Model& model)
{
    auto& graphics = EGEngine->getGraphics();

    uint32_t matIndex{ 0 };
    for (auto& mat : model.materials)
    {
        FMaterialParameters params;
        auto pMaterial = std::make_unique<CMaterial>();

        if (mat.values.find("baseColorTexture") != mat.values.end())
        {
            auto texture = mat.values.at("baseColorTexture");
            pMaterial->addTexture("color_tex", loadTexture(vTextures.at(texture.TextureIndex()), vk::Format::eR8G8B8A8Srgb));
            params.vCompileDefinitions.emplace("HAS_BASECOLORMAP");
        }

        if (mat.values.find("metallicRoughnessTexture") != mat.values.end())
        {
            auto texture = mat.values.at("metallicRoughnessTexture");
            pMaterial->addTexture("rmah_tex", loadTexture(vTextures.at(texture.TextureIndex()), vk::Format::eR8G8B8A8Unorm));
            params.vCompileDefinitions.emplace("HAS_METALLIC_ROUGHNESS");
        }

        if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end())
        {
            auto texture = mat.additionalValues.at("normalTexture");
            params.normalScale = static_cast<float>(texture.TextureScale());
            pMaterial->addTexture("normal_tex", loadTexture(vTextures.at(texture.TextureIndex()), vk::Format::eR8G8B8A8Unorm));
            params.vCompileDefinitions.emplace("HAS_NORMALMAP");
        }

        if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end())
        {
            auto texture = mat.additionalValues.at("occlusionTexture");
            params.occlusionStrenth = static_cast<float>(texture.TextureStrength());
            pMaterial->addTexture("occlusion_tex", loadTexture(vTextures.at(texture.TextureIndex()), vk::Format::eR8G8B8A8Unorm));
            params.vCompileDefinitions.emplace("HAS_OCCLUSIONMAP");
        }

        if (mat.additionalValues.find("displacementGeometryTexture") != mat.additionalValues.end())
        {
            auto texture = mat.additionalValues.at("displacementGeometryTexture");

            const auto factor = texture.json_double_value.find("factor");
            if (factor != std::end(texture.json_double_value))
                params.tessellationFactor = factor->second;
            const auto strength = texture.json_double_value.find("strength");
            if (strength != std::end(texture.json_double_value))
                params.displacementStrength = strength->second;

            pMaterial->addTexture("height_tex", loadTexture(vTextures.at(texture.TextureIndex()), vk::Format::eR8G8B8A8Unorm));
            params.vCompileDefinitions.emplace("HAS_HEIGHTMAP");
        }

        if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end())
        {
            auto texture = mat.additionalValues.at("emissiveTexture");
            pMaterial->addTexture("emissive_tex", loadTexture(vTextures.at(texture.TextureIndex()), vk::Format::eR8G8B8A8Srgb));
            params.vCompileDefinitions.emplace("HAS_EMISSIVEMAP");
        }

        if (mat.additionalValues.find("emissiveFactor") != mat.additionalValues.end())
            params.emissiveFactor = glm::make_vec3(mat.additionalValues.at("emissiveFactor").ColorFactor().data());

        if (mat.values.find("roughnessFactor") != mat.values.end())
            params.roughnessFactor = static_cast<float>(mat.values.at("roughnessFactor").Factor());

        if (mat.values.find("metallicFactor") != mat.values.end())
            params.metallicFactor = static_cast<float>(mat.values.at("metallicFactor").Factor());

        //params.roughnessFactor = 0.2f;
        //params.metallicFactor = 0.9f;

        if (mat.values.find("baseColorFactor") != mat.values.end())
            params.baseColorFactor = glm::make_vec4(mat.values.at("baseColorFactor").ColorFactor().data());

        //TODO: Not supported now
        if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end())
        {
            tinygltf::Parameter param = mat.additionalValues.at("alphaMode");
            if (param.string_value == "BLEND")
                params.alphaMode = EAlphaMode::EBLEND;
            else  if (param.string_value == "MASK")
                params.alphaMode = EAlphaMode::EMASK;
            else
                params.alphaMode = EAlphaMode::EOPAQUE;
        }

        if (mat.additionalValues.find("doubleSided") != mat.additionalValues.end())
            params.doubleSided = mat.additionalValues.at("doubleSided").bool_value;

        if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end())
            params.alphaCutoff = static_cast<float>(mat.additionalValues.at("alphaCutoff").Factor());


        for (auto& [name, data] : mat.extensions)
        {
            auto hash = utl::const_hash(name.c_str(), name.size());
            switch (hash)
            {
            case ext::KHR_materials_clearcoat: {
                params.clearcoatFactor = getDoubleValueOrDefault("clearcoatFactor", data);
                params.clearcoatRoughnessFactor = getDoubleValueOrDefault("clearcoatRoughnessFactor", data);
               
                auto clearcoatTexture = getTextureIndex("clearcoatTexture", data);
                if (clearcoatTexture >= 0)
                {
                    pMaterial->addTexture("clearcoat_tex", loadTexture(vTextures.at(clearcoatTexture), vk::Format::eR8G8B8A8Unorm));
                    params.vCompileDefinitions.emplace("HAS_CLEARCOAT_TEX");
                }

                auto clearcoatRoughnessTexture = getTextureIndex("clearcoatRoughnessTexture", data);
                if (clearcoatRoughnessTexture >= 0)
                {
                    pMaterial->addTexture("clearcoat_rough_tex", loadTexture(vTextures.at(clearcoatRoughnessTexture), vk::Format::eR8G8B8A8Unorm));
                    params.vCompileDefinitions.emplace("HAS_CLEARCOAT_ROUGH_TEX");
                }

                auto clearcoatNormalTexture = getTextureIndex("clearcoatNormalTexture", data);
                if (clearcoatNormalTexture >= 0)
                {
                    pMaterial->addTexture("clearcoat_normal_tex", loadTexture(vTextures.at(clearcoatNormalTexture), vk::Format::eR8G8B8A8Unorm));
                    params.vCompileDefinitions.emplace("HAS_CLEARCOAT_NORMAL_TEX");
                }
            } break;

            case ext::KHR_materials_emissive_strength: {
                params.emissiveStrength = getDoubleValueOrDefault("emissiveStrength", data, 1.0);
            } break;

            case ext::KHR_materials_ior: {
                params.ior = getDoubleValueOrDefault("ior", data, 1.5);
            } break;

            case ext::KHR_materials_iridescence: {
                params.iridescenceFactor = getDoubleValueOrDefault("iridescenceFactor", data);
                params.iridescenceIor = getDoubleValueOrDefault("iridescenceIor", data, 1.3);
                params.iridescenceThicknessMinimum = getDoubleValueOrDefault("iridescenceThicknessMinimum", data, 100.0);
                params.iridescenceThicknessMaximum = getDoubleValueOrDefault("iridescenceThicknessMaximum", data, 400.0);

                auto iridescenceTexture = getTextureIndex("iridescenceTexture", data);
                if (iridescenceTexture >= 0)
                {
                    pMaterial->addTexture("iridescence_tex", loadTexture(vTextures.at(iridescenceTexture), vk::Format::eR8G8B8A8Unorm));
                    params.vCompileDefinitions.emplace("HAS_IRIDESCENCE_TEX");
                }

                auto iridescenceThicknessTexture = getTextureIndex("iridescenceThicknessTexture", data);
                if (iridescenceThicknessTexture >= 0)
                {
                    pMaterial->addTexture("iridescence_thickness_tex", loadTexture(vTextures.at(iridescenceThicknessTexture), vk::Format::eR8G8B8A8Unorm));
                    params.vCompileDefinitions.emplace("HAS_IRIDESCENCE_THICKNESS_TEX");
                }
            } break;

            case ext::KHR_materials_sheen: {
                params.sheenColorFactor = getVec3OrDefault("sheenColorFactor", data);
                params.sheenRoughnessFactor = getDoubleValueOrDefault("sheenRoughnessFactor", data);

                auto sheenColorTexture = getTextureIndex("sheenColorTexture", data);
                if (sheenColorTexture >= 0)
                {
                    pMaterial->addTexture("sheen_tex", loadTexture(vTextures.at(sheenColorTexture), vk::Format::eR8G8B8A8Unorm));
                    params.vCompileDefinitions.emplace("HAS_SHEEN_TEX");
                }

                auto sheenRoughnessTexture = getTextureIndex("sheenRoughnessTexture", data);
                if (sheenRoughnessTexture >= 0)
                {
                    pMaterial->addTexture("sheen_rough_tex", loadTexture(vTextures.at(sheenRoughnessTexture), vk::Format::eR8G8B8A8Unorm));
                    params.vCompileDefinitions.emplace("HAS_SHEEN_ROUGH_TEX");
                }
            } break;

            case ext::KHR_materials_specular: {
                params.specularFactor = getDoubleValueOrDefault("specularFactor", data, 1.0);
                params.specularColorFactor = getVec3OrDefault("specularColorFactor", data, glm::vec3(1.f));

                auto specularTexture = getTextureIndex("specularTexture", data);
                if (specularTexture >= 0)
                {
                    pMaterial->addTexture("specular_tex", loadTexture(vTextures.at(specularTexture), vk::Format::eR8G8B8A8Unorm));
                    params.vCompileDefinitions.emplace("HAS_SPECULAR_TEX");
                }

                auto specularColorTexture = getTextureIndex("specularColorTexture", data);
                if (specularColorTexture >= 0)
                {
                    pMaterial->addTexture("specular_color_tex", loadTexture(vTextures.at(specularColorTexture), vk::Format::eR8G8B8A8Unorm));
                    params.vCompileDefinitions.emplace("HAS_SPECULAR_COLOR_TEX");
                }
            } break;

            case ext::KHR_materials_transmission: {
                params.transmissionFactor = getDoubleValueOrDefault("transmissionFactor", data);

                auto transmissionTexture = getTextureIndex("transmissionTexture", data);
                if (transmissionTexture >= 0)
                {
                    pMaterial->addTexture("transmission_tex", loadTexture(vTextures.at(transmissionTexture), vk::Format::eR8G8B8A8Unorm));
                    params.vCompileDefinitions.emplace("HAS_TRANSMISSION_TEX");
                }
            } break;

            case ext::KHR_materials_unlit: {
            } break;

            case ext::KHR_materials_variants: {
            } break;

            case ext::KHR_materials_volume: {
                params.thicknessFactor = getDoubleValueOrDefault("thicknessFactor", data);
                params.attenuationDistance = getDoubleValueOrDefault("attenuationDistance", data, INFINITY);
                params.attenuationColor = getVec3OrDefault("attenuationColor", data, glm::vec3(1.f));

                auto thicknessTexture = getTextureIndex("thicknessTexture", data);
                if (thicknessTexture >= 0)
                {
                    pMaterial->addTexture("thickness_tex", loadTexture(vTextures.at(thicknessTexture), vk::Format::eR8G8B8A8Unorm));
                    params.vCompileDefinitions.emplace("HAS_THICKNESS_TEX");
                }
            } break;

            default: break;
            }

            log_info("Material with name {} requires glTF 2.0 extention named \"{}\"", mat.name, name);
        }

        // TODO: add instances in shader object
        pMaterial->setParameters(std::move(params));
        auto mat_name = mat.name.empty() ? fs::get_filename(fsParentPath) + "_" + std::to_string(vMaterials.size()) : mat.name;
        if (graphics->getMaterial(mat_name))
            mat_name = mat_name + "_" + std::to_string(matIndex);

        vMaterials.emplace_back(graphics->addMaterial(mat_name, std::move(pMaterial)));
        matIndex++;
    }
}

void CGltfLoader::loadTextures(const tinygltf::Model& model)
{
    for (auto& texture : model.textures)
    {
        bool isBasisU{  };
        auto image_index = texture.source;
        if (image_index < 0 && !texture.extensions.empty())
        {
            auto basisu_support = texture.extensions.find("KHR_texture_basisu");
            if (basisu_support != texture.extensions.end())
            {
                auto& extension = basisu_support->second;
                auto source = extension.Get("source");
                image_index = source.GetNumberAsInt();
                isBasisU = true;
            }
        }

        auto& image = model.images.at(image_index); 
        auto texture_path = fs::path_append(fsParentPath, url_decode(image.uri));
        vTextures.emplace_back(texture_path, isBasisU || fs::is_ktx_format(texture_path));
    }
}

void CGltfLoader::loadAnimations(const tinygltf::Model& model, FSceneComponent* component)
{
    for (auto& anim : model.animations)
    {
        FMeshAnimation animation;
        animation.name = anim.name;

        if (anim.name.empty())
            animation.name = std::to_string(component->animations.size());

        for (auto& samp : anim.samplers)
        {
            FAnimationSampler sampler{};

            if (samp.interpolation == "LINEAR")
                sampler.interpolation = EInterpolationType::eLinear;
            if (samp.interpolation == "STEP")
                sampler.interpolation = EInterpolationType::eStep;
            if (samp.interpolation == "CUBICSPLINE")
                sampler.interpolation = EInterpolationType::eCubicSpline;

            // Read sampler input time values
            {
                const tinygltf::Accessor& accessor = model.accessors[samp.input];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

                float* buf = new float[accessor.count];
                memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(float));
                for (size_t index = 0; index < accessor.count; index++)
                    sampler.inputs.push_back(buf[index]);

                delete buf;

                for (auto input : sampler.inputs)
                {
                    if (input < animation.start)
                        animation.start = input;
                    if (input > animation.end)
                        animation.end = input;
                }
            }

            // Read sampler output T/R/S values 
            {
                const tinygltf::Accessor& accessor = model.accessors[samp.output];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

                switch (accessor.type)
                {
                case TINYGLTF_TYPE_VEC3:
                {
                    glm::vec3* buf = new glm::vec3[accessor.count];
                    memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(glm::vec3));
                    for (size_t index = 0; index < accessor.count; index++)
                        sampler.outputsVec4.push_back(glm::vec4(buf[index], 0.0f));

                    delete buf;
                    break;
                }
                case TINYGLTF_TYPE_VEC4:
                {
                    glm::vec4* buf = new glm::vec4[accessor.count];
                    memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(glm::vec4));
                    for (size_t index = 0; index < accessor.count; index++)
                        sampler.outputsVec4.push_back(buf[index]);

                    delete buf;
                    break;
                }
                default:
                {
                    log_warning("Unknown animation sampler type.");
                    break;
                }
                }
            }

            animation.samplers.push_back(sampler);
        }

        // Channels
        for (auto& source : anim.channels)
        {
            FAnimationChannel channel{};

            if (source.target_path == "rotation")
                channel.path = EPathType::eRotation;
            if (source.target_path == "translation")
                channel.path = EPathType::eTranslation;
            if (source.target_path == "scale")
                channel.path = EPathType::eScale;

            if (source.target_path == "weights") {
                log_warning("Weights channel is not yet supported. Skipping.");
                continue;
            }
            channel.samplerIndex = source.sampler;

            auto target = mIndexToEntity.find(source.target_node);
            if (target != mIndexToEntity.end())
                channel.node = target->second;
            else
                continue;

            animation.channels.push_back(channel);
        }

        component->animations.emplace_back(animation);
    }
}

void CGltfLoader::loadSkins(const tinygltf::Model& model, FSceneComponent* component)
{
    for (auto& source : model.skins)
    {
        FMeshSkin skin{};
        skin.name = source.name;

        // Find skeleton root node
        if (source.skeleton > -1)
        {
            auto target = mIndexToEntity.find(source.skeleton);
            if (target != mIndexToEntity.end())
                skin.root = target->second;
        }

        // Find joint nodes
        for (int jointIndex : source.joints)
        {
            auto target = mIndexToEntity.find(jointIndex);
            if (target != mIndexToEntity.end())
                skin.joints.emplace_back(target->second);
        }

        // Get inverse bind matrices from buffer
        if (source.inverseBindMatrices > -1)
        {
            const tinygltf::Accessor& accessor = model.accessors[source.inverseBindMatrices];
            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
            skin.inverseBindMatrices.resize(accessor.count);
            memcpy(skin.inverseBindMatrices.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(glm::mat4));
        }

        component->skins.emplace_back(skin);
    }
}

size_t CGltfLoader::loadTexture(const std::pair<std::string, bool>& texpair, vk::Format oformat)
{
    auto& graphics = EGEngine->getGraphics();
    auto& device = graphics->getDevice();

    auto name = fs::get_filename(texpair.first);

    if (graphics->getImage(name))
    {
        auto image_id = graphics->getImageID(name);
        graphics->incrementImageUsage(image_id);
        return image_id;
    }

    return graphics->addImage(name, texpair.first, oformat);
}