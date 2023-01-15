#pragma once

#include "ecs/systems/BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CDeferredPassSystem : public ISystem
		{
		public:
			CDeferredPassSystem() { name = "Rendering deferred system"; }
			virtual ~CDeferredPassSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}