#pragma once

#include "ecs/systems/rendering/BaseGraphicsSystem.h"

namespace engine
{
	namespace ecs
	{
		class CEnvironmentSystem : public CBaseGraphicsSystem
		{
		public:
			CEnvironmentSystem() { this->name = "Rendering environment system"; }
			virtual ~CEnvironmentSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		private:
			size_t shader_id{ invalid_index };
		};
	}
}