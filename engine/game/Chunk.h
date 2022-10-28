#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <PerlinNoise.hpp>

namespace engine
{
	namespace game
	{
		class CChunk
		{
		public:

		private:
			uint32_t size{ 500 };
			glm::u32vec3 pos{0};
		};
	}
}