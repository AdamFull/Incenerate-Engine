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

int main()
{
	FEngineCreateInfo ci;
	ci.eAPI = ERenderApi::eVulkan_1_3;

	ci.window.width = 800;
	ci.window.height = 600;
	ci.window.srName = "my window";

	CEngine::getInstance()->create(ci);

	auto& graphics = CEngine::getInstance()->getGraphics();

	std::vector<FVertex> vVertices{};

	siv::PerlinNoise noise{ std::random_device{} };
	noise.reseed(std::mt19937{ 67890u });

	auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	log_info("Voxels traced by: {}s.", std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count());


	CEngine::getInstance()->begin_render_loop();

	return 0;
}