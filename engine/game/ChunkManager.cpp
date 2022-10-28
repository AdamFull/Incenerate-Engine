#include "ChunkManager.h"

using namespace engine::game;

CChunkManager::CChunkManager()
{
    noise = siv::PerlinNoise{ std::random_device{} };
    noise.reseed(std::mt19937{ 67890u });
}

void CChunkManager::create()
{

}