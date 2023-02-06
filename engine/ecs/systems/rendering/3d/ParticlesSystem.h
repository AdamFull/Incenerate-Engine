#pragma once

#include "ecs/systems/rendering/BaseGraphicsSystem.h"

namespace engine
{
	namespace ecs
	{
		class CParticlesSystem : public CBaseGraphicsSystem
		{
		public:
			CParticlesSystem() { name = "Particles system"; }
			virtual ~CParticlesSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}