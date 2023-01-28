#pragma once

#include "ecs/systems/rendering/BaseGraphicsSystem.h"

namespace engine
{
	namespace ecs
	{
		class CBillboardSystem : public CBaseGraphicsSystem
		{
		public:
			CBillboardSystem() { name = "Rendering billboard system"; }
			virtual ~CBillboardSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		private:
			size_t shader_id{ invalid_index };
		};
	}
}