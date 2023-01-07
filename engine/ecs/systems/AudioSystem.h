#pragma once

#include "BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CAudioSystem : public ISystem
		{
		public:
			virtual ~CAudioSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}