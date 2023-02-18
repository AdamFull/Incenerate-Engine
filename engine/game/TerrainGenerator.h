#pragma once

#include "ecs/components/fwd.h"

namespace engine
{
	namespace game
	{
		// Move to primitives?
		class CTerrainLoader
		{
		public:
			void load(ecs::FTerrainComponent* terrain);

		private:
			void loadHeightmap(ecs::FTerrainComponent* terrain);
			float getHeight(uint32_t x, uint32_t y);
		private:
			std::vector<uint16_t> heightdata;
			uint32_t dim;
			uint32_t scale;
		};
	}
}