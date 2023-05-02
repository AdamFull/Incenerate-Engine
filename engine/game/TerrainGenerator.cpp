#include "TerrainGenerator.h"

#include "Engine.h"
#include "graphics/APIStructures.h"
#include "graphics/image/ImageLoader.h"

#include "ecs/components/TerrainComponent.h"
#include "loaders/mesh/MeshHelper.h"

using namespace engine;
using namespace engine::loaders;
using namespace engine::graphics;
using namespace engine::ecs;
using namespace engine::game;

void generateIndices(std::vector<uint32_t>& indices, uint32_t size, bool useQuads)
{
	const uint32_t w = (size - 1u);

	if (useQuads)
		indices.resize(w * w * 4u);
	else
		indices.resize(w * w * 6u);

	for (uint32_t x = 0u; x < w; x++)
	{
		for (uint32_t z = 0u; z < w; z++)
		{
			uint32_t index0 = z * size + x;
			uint32_t index1 = index0 + 1u;
			uint32_t index2 = index0 + size;
			uint32_t index3 = index2 + 1u;

			if (useQuads)
			{
				auto index = (x + z * w) * 4ull;
				indices[index] = index0;
				indices[index + 1ull] = index2;
				indices[index + 2ull] = index3;
				indices[index + 3ull] = index1;
			}
			else
			{
				auto index = (x + z * w) * 6ull;
				indices[index] = index0;
				indices[index + 1ull] = index2;
				indices[index + 2ull] = index1;
				indices[index + 3ull] = index1;
				indices[index + 4ull] = index2;
				indices[index + 5ull] = index3;
			}
		}
	}
}

// Based on https://github.com/SaschaWillems/Vulkan/blob/master/examples/terraintessellation/terraintessellation.cpp
void CTerrainLoader::load(FTerrainComponent* terrain)
{
	auto& graphics = EGEngine->getGraphics();

	terrain->source = "textures\\terrain\\terrain_height.ktx2";
	loadHeightmap(terrain);
	loadMaterial(terrain);

	auto& material = graphics->getMaterial(terrain->material_id);
	auto& params = material->getParameters();
	auto& shader = graphics->getShader(material->getShader());

	std::vector<FVertex> vertices;
	std::vector<uint32_t> indices;

	uint32_t size = terrain->size;

	const uint32_t w = (size - 1u);

	const auto fsize = static_cast<float>(size);

	// Calculate bounding box
	terrain->min = glm::vec3(-fsize, std::numeric_limits<float>::max(), -fsize);
	terrain->max = glm::vec3(fsize, std::numeric_limits<float>::min(), fsize);
	
	vertices.resize(terrain->size * terrain->size);

	for (uint32_t x = 0; x < size; x++)
	{
		for (uint32_t z = 0; z < size; z++)
		{
			uint32_t index = (x + z * size);
			auto& vertex = vertices[index];

			float xPos = static_cast<float>(x) * terrain->grid_scale + terrain->grid_scale / 2.0f - fsize * terrain->grid_scale / 2.0f;
			float zPos = static_cast<float>(z) * terrain->grid_scale + terrain->grid_scale / 2.0f - fsize * terrain->grid_scale / 2.0f;
			float yPos = 0.f;

			// Calculating vertex
			vertex.pos = glm::vec3(xPos, yPos, zPos);
			vertex.texcoord = glm::vec2(static_cast<float>(x) / fsize, static_cast<float>(z) / fsize) * terrain->uv_scale;
			vertex.color = glm::vec3(1.f);
		}
	}

	auto use_tess = shader->getTesselationFlag();

	generateIndices(indices, size, use_tess);

	terrain->vbo_id = graphics->addVertexBuffer("terrain:" + terrain->source);
	auto& vbo = graphics->getVertexBuffer(terrain->vbo_id);
	vbo->addVertices(vertices);
	vbo->addIndices(indices);
	vbo->setLoaded();
}

void CTerrainLoader::loadMaterial(ecs::FTerrainComponent* terrain)
{
	auto& graphics = EGEngine->getGraphics();

	// Load material
	auto material = std::make_unique<CMaterial>();
	FMaterialParameters params{};
	params.tessellationFactor = 0.75f;
	params.displacementStrength = 100.f;
	params.emissiveStrength = 0.f;

	if (terrain->heightmap_id != invalid_index)
	{
		params.vCompileDefinitions.emplace_back("HAS_HEIGHTMAP");
		material->addTexture("height_tex", terrain->heightmap_id);
	}

	params.vCompileDefinitions.emplace_back("HAS_BASECOLORMAP");
	material->addTexture("color_tex", graphics->addImage("terrain_texture", "textures\\terrain\\terrain_diffuse.ktx2"));

	material->setParameters(std::move(params));
	material->incrementUsageCount();
	terrain->material_id = graphics->addMaterial("terrain:" + terrain->source, std::move(material));

	graphics->addShader("terrain_tessellation", "terrain_tessellation", terrain->material_id);
}

void CTerrainLoader::loadHeightmap(FTerrainComponent* terrain)
{
	std::unique_ptr<FImageCreateInfo> imageCI;
	CImageLoader::load(terrain->source, imageCI);

	if (imageCI && !terrain->source.empty())
	{
		if (imageCI->pixFormat != vk::Format::eR16Unorm)
			log_error("Height map format is not supported!");

		heightdata.resize(imageCI->baseWidth * imageCI->baseWidth);

		dim = imageCI->baseWidth;
		scale = dim / terrain->size;

		// Copy image data before loading image to gpu
		memcpy(heightdata.data(), imageCI->pData.get(), imageCI->dataSize);

		// TODO: Add allocate image???
		auto& graphics = EGEngine->getGraphics();
		auto& device = graphics->getDevice();

		auto height_image = std::make_unique<CImage>(device.get());
		height_image->create(imageCI);

		height_size = glm::vec2(imageCI->baseWidth, imageCI->baseHeight);

		terrain->heightmap_id = graphics->addImage(terrain->source, std::move(height_image));
	}
}

float CTerrainLoader::getHeight(uint32_t x, uint32_t y)
{
	glm::ivec2 rpos = glm::ivec2(x, y);
	rpos *= glm::ivec2(scale);
	rpos.x = std::max(0, std::min(rpos.x, (int)dim - 1));
	rpos.y = std::max(0, std::min(rpos.y, (int)dim - 1));
	rpos /= glm::ivec2(scale);
	size_t index = (rpos.x + rpos.y * dim) * scale;
	return *(heightdata.data() + index) / 65535.0f;
}