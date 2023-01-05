#pragma once

#include "ecs/core/System.hpp"

namespace engine
{
	namespace ecs
	{
		class CCompositionSystem : public ISystem
		{
		public:
			virtual ~CCompositionSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}