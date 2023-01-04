#pragma once

#include "ecs/core/System.hpp"

namespace engine
{
	namespace ecs
	{
		class CCameraControlSystem : public ISystem
		{
		public:
			void create() override;
			void update(float fDt) override;
		};
	}
}