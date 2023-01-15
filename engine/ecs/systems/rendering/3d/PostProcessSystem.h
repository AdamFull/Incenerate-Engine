#pragma once

#include "ecs/systems/BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CPostProcessSystem : public ISystem
		{
		public:
			CPostProcessSystem() { name = "Rendering post process system"; }
			virtual ~CPostProcessSystem() override;

			void __create() override;
			void __update(float fDt) override;
		private:
			size_t shader_id{ invalid_index };
		};
	}
}