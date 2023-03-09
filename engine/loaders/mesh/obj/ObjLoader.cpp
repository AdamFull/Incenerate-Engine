#include "ObjLoader.h"

#include "Engine.h"

#include "ecs/components/components.h"
#include "game/SceneGraph.hpp"

#include "system/filesystem/filesystem.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <utility/tinyobjloader.h>

#include "loaders/mesh/MeshHelper.h"

using namespace engine::graphics;
using namespace engine::game;
using namespace engine::system;
using namespace engine::ecs;
using namespace engine::loaders;

void computeSmoothingShape(
	tinyobj::attrib_t& inattrib, tinyobj::shape_t& inshape,
	std::vector<std::pair<unsigned int, unsigned int>>& sortedids,
	unsigned int idbegin, unsigned int idend,
	std::vector<tinyobj::shape_t>& outshapes,
	tinyobj::attrib_t& outattrib)
{
	unsigned int sgroupid = sortedids[idbegin].first;
	bool hasmaterials = inshape.mesh.material_ids.size();
	// Make a new shape from the set of faces in the range [idbegin, idend).
	outshapes.emplace_back();
	tinyobj::shape_t& outshape = outshapes.back();
	outshape.name = inshape.name;
	// Skip lines and points.

	std::unordered_map<unsigned int, unsigned int> remap;
	for (unsigned int id = idbegin; id < idend; ++id) 
	{
		unsigned int face = sortedids[id].second;

		outshape.mesh.num_face_vertices.push_back(3); // always triangles
		if (hasmaterials)
			outshape.mesh.material_ids.push_back(inshape.mesh.material_ids[face]);
		outshape.mesh.smoothing_group_ids.push_back(sgroupid);
		// Skip tags.

		for (unsigned int v = 0; v < 3; ++v) 
		{
			tinyobj::index_t inidx = inshape.mesh.indices[3 * face + v], outidx;
			assert(inidx.vertex_index != -1);
			auto iter = remap.find(inidx.vertex_index);
			// Smooth group 0 disables smoothing so no shared vertices in that case.
			if (sgroupid && iter != remap.end()) 
			{
				outidx.vertex_index = (*iter).second;
				outidx.normal_index = outidx.vertex_index;
				outidx.texcoord_index = (inidx.texcoord_index == -1) ? -1 : outidx.vertex_index;
			}
			else 
			{
				assert(outattrib.vertices.size() % 3 == 0);
				unsigned int offset = static_cast<unsigned int>(outattrib.vertices.size() / 3);
				outidx.vertex_index = outidx.normal_index = offset;
				outidx.texcoord_index = (inidx.texcoord_index == -1) ? -1 : offset;
				outattrib.vertices.push_back(inattrib.vertices[3 * inidx.vertex_index]);
				outattrib.vertices.push_back(inattrib.vertices[3 * inidx.vertex_index + 1]);
				outattrib.vertices.push_back(inattrib.vertices[3 * inidx.vertex_index + 2]);
				outattrib.normals.push_back(0.0f);
				outattrib.normals.push_back(0.0f);
				outattrib.normals.push_back(0.0f);
				if (inidx.texcoord_index != -1) 
				{
					outattrib.texcoords.push_back(inattrib.texcoords[2 * inidx.texcoord_index]);
					outattrib.texcoords.push_back(inattrib.texcoords[2 * inidx.texcoord_index + 1]);
				}
				remap[inidx.vertex_index] = offset;
			}
			outshape.mesh.indices.push_back(outidx);
		}
	}
}

void computeSmoothingShapes(tinyobj::attrib_t& inattrib, std::vector<tinyobj::shape_t>& inshapes, std::vector<tinyobj::shape_t>& outshapes, tinyobj::attrib_t& outattrib)
{
	for (size_t s = 0, slen = inshapes.size(); s < slen; ++s) 
	{
		tinyobj::shape_t& inshape = inshapes[s];

		unsigned int numfaces = static_cast<unsigned int>(inshape.mesh.smoothing_group_ids.size());
		assert(numfaces);
		std::vector<std::pair<unsigned int, unsigned int>> sortedids(numfaces);
		for (unsigned int i = 0; i < numfaces; ++i)
			sortedids[i] = std::make_pair(inshape.mesh.smoothing_group_ids[i], i);
		sort(sortedids.begin(), sortedids.end());

		unsigned int activeid = sortedids[0].first;
		unsigned int id = activeid, idbegin = 0, idend = 0;
		// Faces are now bundled by smoothing group id, create shapes from these.
		while (idbegin < numfaces) 
		{
			while (activeid == id && ++idend < numfaces)
				id = sortedids[idend].first;
			computeSmoothingShape(inattrib, inshape, sortedids, idbegin, idend,
				outshapes, outattrib);
			activeid = id;
			idbegin = idend;
		}
	}
}

void computeAllSmoothingNormals(tinyobj::attrib_t& attrib, std::vector<tinyobj::shape_t>& shapes)
{
	glm::vec3 p[3];
	for (size_t s = 0, slen = shapes.size(); s < slen; ++s) 
	{
		const tinyobj::shape_t& shape(shapes[s]);
		size_t facecount = shape.mesh.num_face_vertices.size();
		assert(shape.mesh.smoothing_group_ids.size());

		for (size_t f = 0, flen = facecount; f < flen; ++f) 
		{
			for (unsigned int v = 0; v < 3; ++v) 
			{
				tinyobj::index_t idx = shape.mesh.indices[3 * f + v];
				assert(idx.vertex_index != -1);
				p[v].x = attrib.vertices[3 * idx.vertex_index];
				p[v].y = attrib.vertices[3 * idx.vertex_index + 1];
				p[v].z = attrib.vertices[3 * idx.vertex_index + 2];
			}

			// cross(p[1] - p[0], p[2] - p[0])
			float nx = (p[1].y - p[0].y) * (p[2].z - p[0].z) - (p[1].z - p[0].z) * (p[2].y - p[0].y);
			float ny = (p[1].z - p[0].z) * (p[2].x - p[0].x) - (p[1].x - p[0].x) * (p[2].z - p[0].z);
			float nz = (p[1].x - p[0].x) * (p[2].y - p[0].y) - (p[1].y - p[0].y) * (p[2].x - p[0].x);

			// Don't normalize here.
			for (unsigned int v = 0; v < 3; ++v) 
			{
				tinyobj::index_t idx = shape.mesh.indices[3 * f + v];
				attrib.normals[3 * idx.normal_index] += nx;
				attrib.normals[3 * idx.normal_index + 1] += ny;
				attrib.normals[3 * idx.normal_index + 2] += nz;
			}
		}
	}

	assert(attrib.normals.size() % 3 == 0);
	for (size_t i = 0, nlen = attrib.normals.size() / 3; i < nlen; ++i) 
	{
		tinyobj::real_t& nx = attrib.normals[3 * i];
		tinyobj::real_t& ny = attrib.normals[3 * i + 1];
		tinyobj::real_t& nz = attrib.normals[3 * i + 2];
		tinyobj::real_t len = sqrtf(nx * nx + ny * ny + nz * nz);
		tinyobj::real_t scale = len == 0 ? 0 : 1 / len;
		nx *= scale;
		ny *= scale;
		nz *= scale;
	}
}

void CObjLoader::load(const std::filesystem::path& source, const entt::entity& pRoot, FSceneComponent* component)
{
	auto& graphics = EGEngine->getGraphics();
	auto& registry = EGEngine->getRegistry();

	auto fpath = std::filesystem::weakly_canonical(fs::get_workdir() / source);
	fsParentPath = std::filesystem::weakly_canonical(fpath.parent_path());
	fsRelPath = std::filesystem::relative(fsParentPath, fs::get_workdir());
	auto filename = fpath.string();
	auto base_dir = fsParentPath.string();

	tinyobj::attrib_t inattrib;
	std::vector<tinyobj::shape_t> inshapes;
	std::vector<tinyobj::material_t> materials;

	std::string warning;
	std::string error;
	bool ret = tinyobj::LoadObj(&inattrib, &inshapes, &materials, &warning, &error, filename.c_str(), base_dir.c_str());

	if (!warning.empty())
		log_warning("\nWarnings while loading gltf scene \"{}\": \n{}", fs::from_unicode(source), warning);
	if (!error.empty())
		log_error("\nErrors while loading gltf scene \"{}\": \n{}", fs::from_unicode(source), error);

	if (ret)
	{
		vbo_id = graphics->addVertexBuffer(fs::from_unicode(fpath.filename()));

		// Load materials
		uint32_t matIndex{ 0 };
		for (auto& material : materials)
		{
			FMaterialParameters params;
			auto pMaterial = std::make_unique<CMaterial>();

			if (!material.diffuse_texname.empty())
			{
				pMaterial->addTexture("color_tex", tryLoadTexture(material.diffuse_texname, vk::Format::eR8G8B8A8Srgb));
				params.vCompileDefinitions.emplace_back("HAS_BASECOLORMAP");
			}

			if (!material.metallic_texname.empty())
			{
				pMaterial->addTexture("metallic_tex", tryLoadTexture(material.metallic_texname, vk::Format::eR8G8B8A8Unorm));
				params.vCompileDefinitions.emplace_back("HAS_METALLIC");
			}

			if (!material.roughness_texname.empty())
			{
				pMaterial->addTexture("roughness_tex", tryLoadTexture(material.roughness_texname, vk::Format::eR8G8B8A8Unorm));
				params.vCompileDefinitions.emplace_back("HAS_ROUGHNESS");
			}

			if (!material.bump_texname.empty())
			{
				pMaterial->addTexture("normal_tex", tryLoadTexture(material.bump_texname, vk::Format::eR8G8B8A8Unorm));
				params.vCompileDefinitions.emplace_back("HAS_NORMALMAP");
			}

			if (!material.normal_texname.empty())
			{
				pMaterial->addTexture("normal_tex", tryLoadTexture(material.normal_texname, vk::Format::eR8G8B8A8Unorm));
				params.vCompileDefinitions.emplace_back("HAS_NORMALMAP");
			}

			if (!material.ambient_texname.empty())
			{
				pMaterial->addTexture("occlusion_tex", tryLoadTexture(material.ambient_texname, vk::Format::eR8G8B8A8Unorm));
				params.vCompileDefinitions.emplace_back("HAS_OCCLUSIONMAP");
			}

			if (!material.displacement_texname.empty())
			{
				pMaterial->addTexture("height_tex", tryLoadTexture(material.displacement_texname, vk::Format::eR8G8B8A8Unorm));
				params.vCompileDefinitions.emplace_back("HAS_HEIGHTMAP");
			}

			if (!material.emissive_texname.empty())
			{
				pMaterial->addTexture("emissive_tex", tryLoadTexture(material.emissive_texname, vk::Format::eR8G8B8A8Unorm));
				params.vCompileDefinitions.emplace_back("HAS_EMISSIVEMAP");
			}

			if (!material.specular_texname.empty())
			{
				pMaterial->addTexture("specular_tex", tryLoadTexture(material.specular_texname, vk::Format::eR8G8B8A8Unorm));
				params.vCompileDefinitions.emplace_back("HAS_SPECULARMAP");
			}

			if (material.dissolve == 1.f)
				params.alphaMode = EAlphaMode::EOPAQUE;
			else  if (material.dissolve == 0.f)
				params.alphaMode = EAlphaMode::EBLEND;
			else
				params.alphaMode = EAlphaMode::EMASK;

			params.alphaCutoff = material.dissolve;
			params.metallicFactor = material.metallic;
			params.roughnessFactor = material.roughness;
			params.ior = material.ior;

			params.baseColorFactor = glm::vec4(glm::make_vec3(material.diffuse), 1.f);

			auto emissiveFactor = glm::make_vec3(material.emission);
			params.emissiveFactor = emissiveFactor == glm::vec3(0.f) ? glm::vec3(1.f) : emissiveFactor;

			pMaterial->setParameters(std::move(params));
			auto mat_name = material.name.empty() ? fs::get_filename(fsParentPath) + "_" + std::to_string(vMaterials.size()) : material.name;
			if (graphics->getMaterial(mat_name))
				mat_name = mat_name + "_" + std::to_string(matIndex);

			vMaterials.emplace_back(graphics->addMaterial(mat_name, std::move(pMaterial)));
			matIndex++;
		}

		// Load shapes
		auto mesh_node = scenegraph::create_node(fpath.filename().string());
		scenegraph::attach_child(pRoot, mesh_node);
		registry.emplace<FMeshComponent>(mesh_node, FMeshComponent{});
		auto& meshComponent = registry.get<FMeshComponent>(mesh_node);

		meshComponent.vbo_id = vbo_id;
		meshComponent.head = pRoot;

		bool regen_all_normals = inattrib.normals.size() == 0;
		tinyobj::attrib_t outattrib;
		std::vector<tinyobj::shape_t> outshapes;

		if (regen_all_normals) 
		{
			computeSmoothingShapes(inattrib, inshapes, outshapes, outattrib);
			computeAllSmoothingNormals(outattrib, outshapes);
		}

		std::vector<tinyobj::shape_t>& shapes = regen_all_normals ? outshapes : inshapes;
		tinyobj::attrib_t& attrib = regen_all_normals ? outattrib : inattrib;

		auto& vbo = graphics->getVertexBuffer(vbo_id);
		for (auto& shape : shapes)
		{
			std::unordered_map<FVertex, uint32_t> uniqueVertices{};
			FMeshlet meshlet{};
			meshlet.begin_index = vbo->getLastIndex();
			meshlet.begin_vertex = vbo->getLastVertex();

			glm::vec3 posMin = glm::vec3(std::numeric_limits<float>::max());
			glm::vec3 posMax = glm::vec3(std::numeric_limits<float>::min());

			std::vector<uint32_t> indexBuffer;
			std::vector<FVertex> vertexBuffer;
			
			for (auto& index : shape.mesh.indices)
			{
				FVertex vertex{};
				vertex.pos = glm::vec3(
					attrib.vertices[3 * index.vertex_index], 
					attrib.vertices[3 * index.vertex_index + 1], 
					attrib.vertices[3 * index.vertex_index + 2]);

				vertex.normal = glm::vec3(
					attrib.normals[3 * index.normal_index], 
					attrib.normals[3 * index.normal_index + 1], 
					attrib.normals[3 * index.normal_index + 2]);

				vertex.texcoord = glm::vec2(
					attrib.texcoords[2 * index.texcoord_index], 
					1.f - attrib.texcoords[2 * index.texcoord_index + 1]);

				vertex.color = glm::vec3(
					attrib.colors[3 * index.vertex_index], 
					attrib.colors[3 * index.vertex_index + 1], 
					attrib.colors[3 * index.vertex_index + 2]);

				// Bounding box calculation
				posMin.x = glm::min(posMin.x, vertex.pos.x);
				posMin.y = glm::min(posMin.y, vertex.pos.y);
				posMin.z = glm::min(posMin.z, vertex.pos.z);
				posMax.x = glm::max(posMax.x, vertex.pos.x);
				posMax.y = glm::max(posMax.y, vertex.pos.y);
				posMax.z = glm::max(posMax.z, vertex.pos.z);

				if (uniqueVertices.count(vertex) == 0) 
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertexBuffer.size());
					vertexBuffer.push_back(vertex);
				}

				indexBuffer.push_back(uniqueVertices[vertex]);
			}

			meshlet.index_count = indexBuffer.size();
			meshlet.vertex_count = vertexBuffer.size();
			meshlet.setDimensions(posMin, posMax);
			meshlet.material = vMaterials[shape.mesh.material_ids.front()];
			auto& pMaterial = graphics->getMaterial(meshlet.material);
			pMaterial->incrementUsageCount();

			calculate_tangents(vertexBuffer, indexBuffer, 0);

			meshComponent.vMeshlets.emplace_back(meshlet);
			vbo->addMeshData(std::move(vertexBuffer), std::move(indexBuffer));
		}

		meshComponent.loaded = true;

		for (auto& mat_id : vMaterials)
			graphics->addShader("default_" + std::to_string(mat_id), "default", mat_id);
	}
}

size_t CObjLoader::tryLoadTexture(const std::filesystem::path& path, vk::Format oformat)
{
	auto filepath = fsRelPath / path;
	auto is_ktx = fs::is_ktx_format(filepath);
	return loadTexture(filepath, !is_ktx, oformat);
}

size_t CObjLoader::loadTexture(const std::filesystem::path& path, bool overridef, vk::Format oformat)
{
	auto& graphics = EGEngine->getGraphics();
	auto& device = graphics->getDevice();
	auto pImage = std::make_unique<CImage>(device.get());

	if (!overridef)
		pImage->create(path);
	else
		pImage->create(path, oformat);

	auto name = fs::get_filename(path);
	return graphics->addImage(name, std::move(pImage));
}