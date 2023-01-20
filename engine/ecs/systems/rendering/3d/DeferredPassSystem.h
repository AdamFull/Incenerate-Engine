#pragma once

#include "ecs/systems/rendering/BaseGraphicsSystem.h"

namespace engine
{
	namespace ecs
	{
		class CDeferredPassSystem : public CBaseGraphicsSystem
		{
		public:
			CDeferredPassSystem() { name = "Rendering deferred system"; }
			virtual ~CDeferredPassSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}