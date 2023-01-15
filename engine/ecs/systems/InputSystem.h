#pragma once

#include "BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CInputSystem : public ISystem
		{
		public:
			CInputSystem() { name = "Input system"; }
			virtual ~CInputSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}