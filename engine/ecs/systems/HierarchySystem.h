#pragma once

#include "BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CHierarchySystem : public ISystem
		{
		public:
			virtual ~CHierarchySystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}