#include "engine/engine.h"
#include "engine/system/filesystem/filesystem.h"
#include "engine/graphics/image/Image.h"

#include <random>
#include <limits>
#include <PerlinNoise.hpp>

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

// a regular node
struct svo_node {
	svo_node* children[8];
};

// a node that stores colors instead
struct svo_array_node {
	uint32_t colors[8];
};

// a node that stores just one color, something that we want to avoid
struct svo_leaf_node {
	uint32_t color;
};

uint64_t ileave_two0(uint32_t input)
{
	constexpr size_t numInputs = 3;
	constexpr uint64_t masks[] = 
	{
		0x9249'2492'4924'9249,
		0x30C3'0C30'C30C'30C3,
		0xF00F'00F0'0F00'F00F,
		0x00FF'0000'FF00'00FF,
		0xFFFF'0000'0000'FFFF
	};

	uint64_t n = input;
	for (int i = 4; i != 1; --i) 
	{
		const auto shift = (numInputs - 1) * (1 << i);
		n |= n << shift;
		n &= masks[i];
	}

	return n;
}

uint64_t ileave3(const glm::u32vec3 pos)
{
	return (ileave_two0(pos.x) << 2) | (ileave_two0(pos.y) << 1) | ileave_two0(pos.z);
}

constexpr uint32_t chunk_size{ 50 };
constexpr float chunk_divider{ 32.f };
constexpr float voxel_scale{ 0.25f };

glm::vec3 toWorldSpace(const glm::u32vec3& pos)
{
	return glm::vec3(pos) * voxel_scale;
}

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

	std::vector<FVertex> vVertices{};

	siv::PerlinNoise noise{ std::random_device{} };
	noise.reseed(std::mt19937{ 67890u });

	auto startTime = std::chrono::high_resolution_clock::now();

	auto i = ileave3({ 0, 2, 10 });
	vVertices.emplace_back(FVertex{ camera->getViewPos() * 2.f, 0xFF0000FF });

	auto currentTime = std::chrono::high_resolution_clock::now();
	log_info("Voxels traced by: {}s.", std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count());




	auto& program = graphics->addStage("voxel_shader");

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