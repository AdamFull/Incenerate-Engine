#pragma once

#include "ecs/systems/rendering/BaseGraphicsSystem.h"

namespace engine
{
	namespace ecs
	{
		class CReflectionsSystem : public CBaseGraphicsSystem
		{
		public:
			CReflectionsSystem() { name = "Rendering reflections system"; }
			virtual ~CReflectionsSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		private:
			size_t shader_id{ invalid_index };
		};
	}
}