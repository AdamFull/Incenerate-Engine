#pragma once

#include "ecs/systems/BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CCompositionSystem : public ISystem
		{
		public:
			CCompositionSystem() { name = "Rendering composition system"; }
			virtual ~CCompositionSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		private:
			size_t brdflut_id{ invalid_index };
			size_t empty_cube_id{ invalid_index };
			size_t shader_id{ invalid_index };
			size_t stageId{ invalid_index };
		};
	}
}