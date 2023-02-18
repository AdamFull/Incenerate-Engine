#pragma once

#include "ecs/systems/rendering/BaseGraphicsSystem.h"

namespace engine
{
	namespace ecs
	{
		class CTerrainSystem : public CBaseGraphicsSystem
		{
		public:
			CTerrainSystem() { name = "Terrain rendering system"; }
			virtual ~CTerrainSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}