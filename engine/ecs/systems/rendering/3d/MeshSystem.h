#pragma once

#include "ecs/systems/BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CMeshSystem : public ISystem
		{
		public:
			virtual ~CMeshSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}