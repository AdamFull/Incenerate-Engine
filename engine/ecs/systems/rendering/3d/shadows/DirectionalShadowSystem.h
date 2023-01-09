#pragma once

#include "ecs/systems/BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CDirectionalShadowSystem : public ISystem
		{
		public:
			virtual ~CDirectionalShadowSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		private:
			size_t shader_id{ invalid_index };
		};
	}
}