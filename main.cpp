#include "engine/engine.h"
#include "engine/system/filesystem/filesystem.h"
#include "engine/graphics/image/Image.h"

#include <random>
#include <limits>
#include <PerlinNoise.hpp>

#include "engine/graphics/VoxelBufferObject.h"

#define OGT_VOX_IMPLEMENTATION
#include <ogt_vox.h>

using namespace engine;
using namespace engine::graphics;
using namespace engine::system;

uint32_t ConvertToRGBA(const glm::vec4& color)
{
	auto r = (uint8_t)(color.x * 255.f);
	auto g = (uint8_t)(color.y * 255.f);
	auto b = (uint8_t)(color.z * 255.f);
	auto a = (uint8_t)(color.w * 255.f);

	return (a << 24) | (b << 16) | (g << 8) | r;
}

uint32_t ConvertToRGBA(const ogt_vox_rgba& color)
{
	return (color.a << 24) | (color.b << 16) | (color.g << 8) | color.r;
}

constexpr uint32_t chunk_size{ 50 };
constexpr float chunk_divider{ 32.f };
constexpr float voxel_scale{ 0.25f };

glm::vec3 toWorldSpace(const glm::u32vec3& pos)
{
	return glm::vec3(pos) * voxel_scale;
}

struct FBox
{
	FBox() = default;
	FBox(const glm::vec3& position)
	{
		set(position);
	}

	void set(const glm::vec3& position)
	{
		min = (glm::vec3{ -voxel_scale } + position);
		max = (glm::vec3{ voxel_scale } + position);
	}

	glm::vec3 min, max;
};

struct FRay
{
	FRay() = default;
	FRay(const glm::vec3& origin, const glm::vec3& direction)
	{
		set(origin, direction);
	}

	void set(const glm::vec3& origin, const glm::vec3& direction)
	{
		this->origin = origin;
		this->direction = direction;
	}

	glm::vec3 origin, direction;
};

class CChunk
{
public:
	CChunk(const glm::vec3& pos)
	{
		chunk_pos = pos;
	}

	CChunk(const glm::u32vec3& pos)
	{
		chunk_pos = glm::vec3(pos * chunk_size) * voxel_scale;
	}

	bool check(const glm::vec3& voxel_pos, const siv::PerlinNoise& noise)
	{
		auto noisep = voxel_pos / chunk_divider;

		auto height = (noise.octave2D_01(noisep.x, noisep.z, 1) * (float)chunk_size) * voxel_scale;

		return voxel_pos.y <= height;
	}

	void make(const siv::PerlinNoise& noise, std::vector<FVertex>& verts, const std::unique_ptr<CCamera>& camera)
	{
		float distance{ 0.f };
		FBox box; FRay ray;
		ray.origin = camera->getViewPos();

		for (uint32_t x = 0; x < chunk_size; x++)
		{
			for (uint32_t y = 0; y < chunk_size; y++)
			{
				for (uint32_t z = 0; z < chunk_size; z++)
				{
					auto final_pos = (glm::vec3(x, y, z) * voxel_scale) + chunk_pos;
					if (check(final_pos, noise))
					{
						uint32_t color = 0xFF008000;

						ray.direction = glm::normalize(final_pos - ray.origin);
						box.set(final_pos);
						if (intersect(box, ray, distance))
						{
							verts.emplace_back(FVertex(ray.origin, color));
							verts.emplace_back(FVertex(ray.direction * 50.f, color));
						}
						
						//if(intersect(box, ray, distance))
						//	verts.emplace_back(FVertex(final_pos, color));
					}
				}
			}
		}
	}

	bool intersect(const FBox& box, const FRay& ray, float& distance)
	{
		auto t1 = (box.min - ray.origin) / ray.direction;
		auto t2 = (box.max - ray.origin) / ray.direction;

		auto tmin = glm::max(glm::max(glm::min(t1.x, t2.x), glm::min(t1.y, t2.y)), glm::min(t1.z, t2.z));
		auto tmax = glm::min(glm::min(glm::max(t1.x, t2.x), glm::max(t1.y, t2.y)), glm::max(t1.z, t2.z));

		if (tmax < 0 || tmin > tmax)
		{
			distance = tmax;
			return false;
		}

		distance = tmin;
		return true;
	}

	const glm::vec3& start() { return chunk_pos; }
	const glm::vec3& end() { return chunk_pos + glm::vec3(1.f, 1.f, 1.f) * (float)chunk_size * voxel_scale; }

private:
	glm::vec3 chunk_pos{ 0.f };
};

struct FTreeNode
{
	uint32_t data{ 0 };

	bool isLeaf() { return false; }
	bool isBranch() { return false; }
};

int main()
{
	FEngineCreateInfo ci;
	ci.eAPI = ERenderApi::eVulkan_1_1;

	ci.window.width = 800;
	ci.window.height = 600;
	ci.window.srName = "my window";

	CEngine::getInstance()->create(ci);

	auto& graphics = CEngine::getInstance()->getGraphics();
	auto& cameractrl = CEngine::getInstance()->getCameraController();
	auto& camera = cameractrl->getCamera();
	cameractrl->update(0.f);

	std::vector<CChunk> vChunks{};
	std::vector<FVertex> vVertices{};

	siv::PerlinNoise noise{ std::random_device{} };
	noise.reseed(std::mt19937{ 67890u });

	vVertices.reserve(500 * 500 * 500);

	for (uint32_t x = 0; x < 3; x++)
	{
		for (uint32_t z = 0; z < 3; z++)
		{
			vChunks.emplace_back(CChunk(glm::u32vec3{ x, 0, z }));
		}
	}

	auto startTime = std::chrono::high_resolution_clock::now();

	auto& frustum = camera->getFrustum();
	for (auto& chunk : vChunks)
	{
		if(frustum.checkBox(chunk.start(), chunk.end()))
			chunk.make(noise, vVertices, camera);
	}

	auto currentTime = std::chrono::high_resolution_clock::now();
	log_info("Voxels traced by: {}s.", std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count());

	//Get chunk min and chunk max
	//Iterate between min and max

	//vVertices.emplace_back(FVertex{ camera->getViewPos() * 2.f, 0xFF0000FF});

	//auto voxfile = fs::read_file("models/monu2.vox");
	//
	//const ogt_vox_scene* scene = ogt_vox_read_scene_with_flags(reinterpret_cast<const uint8_t*>(voxfile.c_str()), voxfile.size(), 0);
	//if (scene)
	//{
	//    for (uint32_t instance_index = 0; instance_index < scene->num_instances; instance_index++)
	//    {
	//        const ogt_vox_instance* instance = &scene->instances[instance_index];
	//        const ogt_vox_model* model = scene->models[instance->model_index];
	//
	//        uint32_t voxel_index = 0;
	//        for (uint32_t z = 0; z < model->size_z; z++) 
	//        {
	//            auto zc = (float)z * 0.25f;
	//            for (uint32_t y = 0; y < model->size_y; y++) 
	//            {
	//                auto yc = (float)y * 0.25f;
	//                for (uint32_t x = 0; x < model->size_x; x++, voxel_index++)
	//                {
	//                    uint32_t color_index = model->voxel_data[voxel_index];
	//
	//                    if (color_index != 0)
	//                    {
	//                        auto xc = (float)x * 0.25f;
	//                        auto color = ConvertToRGBA(scene->palette.color[color_index]);
	//                        vVertices.emplace_back(FVertex(glm::vec3(xc, yc, zc), color));
	//                    }
	//                }
	//            }
	//        }
	//    }
	//}
	//
	//ogt_vox_destroy_scene(scene);

	

	//auto image = std::make_unique<CImage>(graphics->getDevice().get());
	//image->create("lava-and-rock_albedo.ktx2");
	//
	//auto& program = graphics->addStage("screenspace");
	//
	//program->setRenderFunc([&](CShaderObject* pso, vk::CommandBuffer& cb)
	//    {
	//        pso->addTexture("input_tex", image->getDescriptor());
	//    });

	auto& program = graphics->addStage("test_shader");

	auto& vbo = program->getVBO();
	vbo->addVertices(std::move(vVertices));

	program->setRenderFunc([&](CShaderObject* pso, vk::CommandBuffer& cb)
		{
			auto projection = camera->getProjection();
			auto view = camera->getView();

			auto& ubo = pso->getUniformBuffer("MatricesBlock");
			ubo->set("projView", projection * view);
		});

	CEngine::getInstance()->begin_render_loop();

	return 0;
}