#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include "GltfLoader.h"
#include "Engine.h"
#include "game/SceneGraph.hpp"

#include "graphics/rendering/material/Material.h"

#include "ecs/components/components.h"

#include "system/filesystem/filesystem.h"

#include <utility/logger/logger.h>
#include <glm/gtc/type_ptr.hpp>

using namespace engine::loaders;
using namespace engine::system;
using namespace engine::graphics;
using namespace engine::game;
using namespace engine::ecs;


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

// Based on https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanglTFModel.cpp
bool loadImageDataFuncEmpty(tinygltf::Image* image, const int imageIndex, std::string* err, std::string* warn, int req_width, int req_height, const unsigned char* bytes, int size, void* userData)
{
    // KTX files will be handled by our own code
    //if (image->uri.find_last_of(".") != std::string::npos)
    //{
    //    if (image->uri.substr(image->uri.find_last_of(".") + 1) == "ktx")
    //        return true;
    //    if (image->uri.substr(image->uri.find_last_of(".") + 1) == "ktx2")
    //        return true;
    //
    //}

    return true;//tinygltf::LoadImageData(image, imageIndex, error, warning, req_width, req_height, bytes, size, userData);
}

void CGltfLoader::load(const const std::filesystem::path& source, const entt::entity& root)
{
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF gltfContext;
    std::string error, warning;
    gltfContext.SetImageLoader(loadImageDataFuncEmpty, nullptr);

    auto fpath = std::filesystem::weakly_canonical(fs::get_workdir() / source);
    fsParentPath = std::filesystem::weakly_canonical(fpath.parent_path());
    fsParentPath = std::filesystem::relative(fsParentPath, fs::get_workdir());

    bool fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, fs::from_unicode(fpath));

    if (!warning.empty())
        log_warning("\nWarnings while loading gltf scene \"{}\": \n{}", fs::from_unicode(source), warning);
    if (!error.empty())
        log_error("\nErrors while loading gltf scene \"{}\": \n{}", fs::from_unicode(source), error);

    if (fileLoaded)
    {
        vbo_id = EGGraphics->addVertexBuffer(fs::from_unicode(fpath.filename()));

        loadTextures(gltfModel);
        loadMaterials(gltfModel);

        const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];

        for (size_t i = 0; i < scene.nodes.size(); i++)
        {
            const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
            loadNode(root, node, scene.nodes[i], gltfModel, 1.0);
        }

        auto& pVBO = EGGraphics->getVertexBuffer(vbo_id);
        pVBO->create();

        for (auto& mat_id : vMaterials)
            EGGraphics->addShader("default_" + std::to_string(mat_id), "default", mat_id);
    }
}

void CGltfLoader::loadNode(const entt::entity& parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, float globalscale)
{
    auto& registry = EGCoordinator;

    auto& phierarchy = registry.get<FHierarchyComponent>(parent);

    auto obj_name = node.name.empty() ? phierarchy.name + "_" + std::to_string(nodeIndex) : node.name;
    auto entity = scenegraph::create_node(obj_name);

    auto& transform = registry.get<FTransformComponent>(entity);

    // Loading position data
    if (node.translation.size() == 3)
        transform.position = glm::make_vec3(node.translation.data());
    // Loading rotation data
    if (node.rotation.size() == 4)
    {
        //TODO: refactor transform for using quaterion
        glm::quat quat = glm::conjugate(glm::make_quat(node.rotation.data()));
        transform.rotation = glm::eulerAngles(quat); //  * glm::vec3(-1.0, 1.0, 1.0)
    }
    // Loading scale data
    if (node.scale.size() == 3)
        transform.scale = glm::make_vec3(node.scale.data());

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
    auto& registry = EGCoordinator;
    const tinygltf::Mesh mesh = model.meshes[node.mesh];
    auto& pVBO = EGGraphics->getVertexBuffer(vbo_id);

    registry.emplace<FMeshComponent>(parent, FMeshComponent{});
    auto& meshComponent = registry.get<FMeshComponent>(parent);

    for (size_t j = 0; j < mesh.primitives.size(); j++)
    {
        std::vector<uint32_t> indexBuffer;
        std::vector<FVertex> vertexBuffer;

        const tinygltf::Primitive& primitive = mesh.primitives[j];
        if (primitive.indices < 0)
            continue;

        uint32_t indexStart = pVBO->getLastIndex();
        uint32_t vertexStart = pVBO->getLastVertex();
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
                    case 4:
                        vert.color = glm::make_vec4(&bufferColors[v * 4]);
                    }
                }
                else
                {
                    vert.color = glm::vec4(1.0f);
                }
                vert.tangent = bufferTangents ? glm::vec4(glm::make_vec4(&bufferTangents[v * 4])) : glm::vec4(0.0f);
                //vert.tangent = glm::vec4(0.f);
                // TODO: Add skinning
                //vert.joint0 = bHasSkin ? glm::vec4(glm::make_vec4(&bufferJoints[v * 4])) : glm::vec4(0.0f);
                //vert.weight0 = bHasSkin ? glm::make_vec4(&bufferWeights[v * 4]) : glm::vec4(0.0f);
                vertexBuffer.push_back(vert);
            }
        }

        // Indices
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

        FMeshlet meshlet;
        meshlet.begin_index = indexStart;
        meshlet.index_count = indexCount;
        meshlet.begin_vertex = vertexStart;
        meshlet.vertex_count = vertexCount;
        meshlet.setDimensions(posMin, posMax);

        if (!vMaterials.empty())
        {
            auto& material = primitive.material > -1 ? vMaterials.at(primitive.material) : vMaterials.back();
            auto& pMaterial = EGGraphics->getMaterial(material);
            pMaterial->incrementUsageCount();
            auto& params = pMaterial->getParameters();
            if (bHasNormals) params.vCompileDefinitions.emplace_back("HAS_NORMALS");
            if (bHasTangents) params.vCompileDefinitions.emplace_back("HAS_TANGENTS");
            meshlet.material = material;
        }

        meshComponent.vMeshlets.emplace_back(meshlet);
        pVBO->addMeshData(std::move(vertexBuffer), std::move(indexBuffer));
    }

    meshComponent.vbo_id = vbo_id;
    meshComponent.loaded = true;
}

void CGltfLoader::loadCameraComponent(const entt::entity& parent, const tinygltf::Node& node, const tinygltf::Model& model)
{
    auto& registry = EGCoordinator;
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
    auto& registry = EGCoordinator;
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
    for (auto& mat : model.materials)
    {
        FMaterial params;
        auto pMaterial = std::make_unique<CMaterial>();

        if (mat.values.find("baseColorTexture") != mat.values.end())
        {
            auto texture = mat.values.at("baseColorTexture");
            pMaterial->addTexture("color_tex", loadTexture(vTextures.at(texture.TextureIndex()), vk::Format::eR8G8B8A8Unorm));
            params.vCompileDefinitions.emplace_back("HAS_BASECOLORMAP");
        }

        if (mat.values.find("metallicRoughnessTexture") != mat.values.end())
        {
            auto texture = mat.values.at("metallicRoughnessTexture");
            pMaterial->addTexture("rmah_tex", loadTexture(vTextures.at(texture.TextureIndex()), vk::Format::eR8G8B8A8Unorm));
            params.vCompileDefinitions.emplace_back("HAS_METALLIC_ROUGHNESS");
        }

        if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end())
        {
            auto texture = mat.additionalValues.at("normalTexture");
            params.normalScale = static_cast<float>(texture.TextureScale());
            pMaterial->addTexture("normal_tex", loadTexture(vTextures.at(texture.TextureIndex()), vk::Format::eR8G8B8A8Unorm));
            params.vCompileDefinitions.emplace_back("HAS_NORMALMAP");
        }

        if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end())
        {
            auto texture = mat.additionalValues.at("occlusionTexture");
            params.occlusionStrenth = static_cast<float>(texture.TextureStrength());
            pMaterial->addTexture("occlusion_tex", loadTexture(vTextures.at(texture.TextureIndex()), vk::Format::eR8G8B8A8Unorm));
            params.vCompileDefinitions.emplace_back("HAS_OCCLUSIONMAP");
        }

        if (mat.additionalValues.find("displacementGeometryTexture") != mat.additionalValues.end())
        {
            auto texture = mat.additionalValues.at("displacementGeometryTexture");

            const auto factor = texture.json_double_value.find("factor");
            if (factor != std::end(texture.json_double_value))
                params.tessellationFactor = factor->second;
            const auto strength = texture.json_double_value.find("strength");
            if (strength != std::end(texture.json_double_value))
                params.tessStrength = strength->second;

            pMaterial->addTexture("height_tex", loadTexture(vTextures.at(texture.TextureIndex()), vk::Format::eR8G8B8A8Unorm));
            params.vCompileDefinitions.emplace_back("HAS_HEIGHTMAP");
        }

        if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end())
        {
            auto texture = mat.additionalValues.at("emissiveTexture");
            pMaterial->addTexture("emissive_tex", loadTexture(vTextures.at(texture.TextureIndex()), vk::Format::eR8G8B8A8Srgb));
            params.vCompileDefinitions.emplace_back("HAS_EMISSIVEMAP");
        }

        if (mat.additionalValues.find("emissiveFactor") != mat.additionalValues.end())
            params.emissiveFactor = glm::make_vec3(mat.additionalValues.at("emissiveFactor").ColorFactor().data());

        if (mat.values.find("roughnessFactor") != mat.values.end())
            params.roughnessFactor = static_cast<float>(mat.values.at("roughnessFactor").Factor());

        if (mat.values.find("metallicFactor") != mat.values.end())
            params.metallicFactor = static_cast<float>(mat.values.at("metallicFactor").Factor());

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
            if (name == "KHR_materials_emissive_strength")
            {
                auto source = data.Get("emissiveStrength");
                params.emissiveStrength = source.GetNumberAsDouble();
            }
                
        }

        // TODO: add instances in shader object
        pMaterial->setParameters(std::move(params));
        auto mat_name = mat.name.empty() ? fs::get_filename(fsParentPath) + "_" + std::to_string(vMaterials.size()) : mat.name;
        vMaterials.emplace_back(EGGraphics->addMaterial(mat_name, std::move(pMaterial)));
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
        auto texture_path = std::filesystem::weakly_canonical(fsParentPath / url_decode(image.uri));
        vTextures.emplace_back(fs::from_unicode(texture_path), isBasisU || fs::is_ktx_format(texture_path));
    }
}

size_t CGltfLoader::loadTexture(const std::pair<std::filesystem::path, bool>& texpair, vk::Format oformat)
{
    auto& device = EGGraphics->getDevice();
    auto pImage = std::make_unique<CImage>(device.get());

    if (texpair.second)
        pImage->create(texpair.first); 
    else
        pImage->create(texpair.first, oformat);

    auto name = fs::get_filename(texpair.first);
    return EGGraphics->addImage(name, std::move(pImage));
}