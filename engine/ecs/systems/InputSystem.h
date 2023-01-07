#pragma once

#include "BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CInputSystem : public ISystem
		{
		public:
			virtual ~CInputSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}