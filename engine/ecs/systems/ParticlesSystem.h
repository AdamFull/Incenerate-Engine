#pragma once

#include "BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CParticlesSystem : public ISystem
		{
		public:
			CParticlesSystem() { name = "Particles system"; }
			virtual ~CParticlesSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}