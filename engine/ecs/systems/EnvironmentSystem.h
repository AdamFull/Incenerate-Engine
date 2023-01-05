#pragma once

#include "ecs/core/System.hpp"

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
		};
	}
}