#pragma once

#include "ecs/core/System.hpp"

namespace engine
{
	namespace ecs
	{
		class CCompositionSystem : public ISystem
		{
		public:
			void create() override;
			void update(float fDt) override;
		};
	}
}