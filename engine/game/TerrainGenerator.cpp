#include "TerrainGenerator.h"

#include "Engine.h"
#include "graphics/APIStructures.h"
#include "loaders/ImageLoader.h"

#include "ecs/components/TerrainComponent.h"

using namespace engine::loaders;
using namespace engine::graphics;
using namespace engine::ecs;
using namespace engine::game;

// Based on https://github.com/SaschaWillems/Vulkan/blob/master/examples/terraintessellation/terraintessellation.cpp
void CTerrainLoader::load(FTerrainComponent* terrain)
{
	terrain->source = "textures\\terrain\\terrain_height.ktx2";
	loadHeightmap(terrain);

	std::vector<FVertex> vertices;
	std::vector<uint32_t> indices;

	uint32_t size = terrain->size;

	const uint32_t w = (size - 1u);
	const float wx = 2.0f;
	const float wz = 2.0f;

	auto sizel = static_cast<size_t>(size);
	vertices.resize(sizel * sizel);

	const auto fsize = static_cast<float>(size);

	// Calculate bounding box
	terrain->min = glm::vec3(-fsize, std::numeric_limits<float>::max(), -fsize);
	terrain->max = glm::vec3(fsize, std::numeric_limits<float>::min(), fsize);

	// Calculate vertices
	for (uint32_t x = 0; x < size; x++)
	{
		for (uint32_t z = 0; z < size; z++)
		{
			uint32_t index = (x + z * size);
			auto& vertex = vertices[index];

			float xPos = static_cast<float>(x) * wx + wx / 2.0f - fsize * wx / 2.0f;
			float zPos = static_cast<float>(z) * wz + wz / 2.0f - fsize * wz / 2.0f;

			// Calculating vertex
			vertex.pos = glm::vec3(xPos, 0.0f, zPos);
			vertex.texcoord = glm::vec2(static_cast<float>(x) / fsize, static_cast<float>(z) / fsize) * terrain->uv_scale;
			vertex.color = glm::vec3(1.f);

			// Load normals from heightmap
			if (terrain->heightmap_id != invalid_index)
			{
				std::array<std::array<float, 3>, 3> heights;
				for (auto hx = -1; hx <= 1; hx++)
				{
					for (auto hz = -1; hz <= 1; hz++)
						heights[hx + 1][hz + 1] = getHeight(x + hx, z + hz);
				}

				auto loc_height = getHeight(x, z);
				terrain->min.y = glm::min(terrain->min.y, loc_height);
				terrain->max.y = glm::max(terrain->max.y, loc_height);

				vertex.normal.x = heights[0][0] - heights[2][0] + 2.0f * heights[0][1] - 2.0f * heights[2][1] + heights[0][2] - heights[2][2];
				vertex.normal.z = heights[0][0] + 2.0f * heights[1][0] + heights[2][0] - heights[0][2] - 2.0f * heights[1][2] - heights[2][2];
				vertex.normal.y = 0.25f * glm::sqrt(1.0f - vertex.normal.x * vertex.normal.x - vertex.normal.z * vertex.normal.z);
				vertex.normal = glm::normalize(vertex.normal * glm::vec3(2.0f, 1.0f, 2.0f));
			}
			else
				vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f); // Setting normal by default
		}
	}

	// Calculating indices
	bool quads{ true };
	if (quads)
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

			if (quads)
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

	// Create vertex buffer
	auto& graphics = EGEngine->getGraphics();

	terrain->vbo_id = graphics->addVertexBuffer("terrain:" + terrain->source);
	auto& vbo = graphics->getVertexBuffer(terrain->vbo_id);
	vbo->addVertices(vertices);
	vbo->addIndices(indices);
	vbo->setLoaded();

	// Load material
	auto material = std::make_unique<CMaterial>();
	FMaterial params{};
	params.tessellationFactor = 1.f;
	params.tessStrength = 1.f;
	//params.vCompileDefinitions.emplace_back("USE_TESSELLATION");

	if (terrain->heightmap_id != invalid_index)
	{
		params.vCompileDefinitions.emplace_back("HAS_HEIGHTMAP");
		material->addTexture("height_tex", terrain->heightmap_id);
	}

	params.vCompileDefinitions.emplace_back("HAS_BASECOLORMAP"); //color_tex
	material->addTexture("height_tex", graphics->addImage("terrain_texture", "textures\\terrain\\terrain_diffuse.ktx2"));

	material->setParameters(std::move(params));
	material->incrementUsageCount();
	terrain->material_id = graphics->addMaterial("terrain:" + terrain->source, std::move(material));

	graphics->addShader("terrain", "terrain", terrain->material_id);
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

		terrain->heightmap_id = graphics->addImage(terrain->source, std::move(height_image));
	}
}

float CTerrainLoader::getHeight(uint32_t x, uint32_t y)
{
	glm::ivec2 rpos = glm::ivec2(x, y) * glm::ivec2(scale);
	rpos.x = std::max(0, std::min(rpos.x, (int)dim - 1));
	rpos.y = std::max(0, std::min(rpos.y, (int)dim - 1));
	rpos /= glm::ivec2(scale);
	return *(heightdata.data() + (rpos.x + rpos.y * dim) * scale) / 65535.0f;
}