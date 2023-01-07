#pragma once

#include "BaseSystem.h"

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
		private:
			size_t shader_id{ invalid_index };
			size_t stageId{ invalid_index };
		};
	}
}