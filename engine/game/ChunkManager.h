#pragma once 

#include "Chunk.h"

namespace engine
{
	namespace game
	{
		class CChunkManager
		{
		public:
			CChunkManager();

			void create();

		private:
			siv::PerlinNoise noise;
			std::vector<CChunk> vChunks;
		};
	}
}