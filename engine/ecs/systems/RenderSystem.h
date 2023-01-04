#pragma once

#include "ecs/core/System.hpp"

namespace engine
{
	namespace ecs
	{
		class CRenderSystem : public ISystem
		{
		public:
			void create() override;
			void update(float fDt) override;
		};
	}
}