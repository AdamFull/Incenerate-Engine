#pragma once

#include "BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CPostProcessSystem : public ISystem
		{
		public:
			virtual ~CPostProcessSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		private:
			size_t shader_id{ invalid_index };
		};
	}
}