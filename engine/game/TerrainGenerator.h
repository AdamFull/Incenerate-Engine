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
			void loadMaterial(ecs::FTerrainComponent* terrain);
			void loadHeightmap(ecs::FTerrainComponent* terrain);
			float getHeight(uint32_t x, uint32_t y);
		private:
			std::vector<uint16_t> heightdata;
			glm::vec2 height_size{ 0.f };
			uint32_t dim;
			uint32_t scale;
		};
	}
}