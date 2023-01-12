#pragma once

#include "ecs/systems/BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CEnvironmentSystem : public ISystem
		{
		public:
			virtual ~CEnvironmentSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		private:
			size_t stageId{ invalid_index };
		};
	}
}