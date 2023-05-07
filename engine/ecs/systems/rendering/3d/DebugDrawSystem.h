#pragma once

#include "ecs/systems/rendering/BaseGraphicsSystem.h"

namespace engine
{
	namespace ecs
	{
		class CDebugDrawSystem : public CBaseGraphicsSystem
		{
		public:
			CDebugDrawSystem() { name = "Debug rendering system"; }
			virtual ~CDebugDrawSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		private:
			size_t normals_shader_id{ invalid_index };
			
			size_t debug_vbo_id{ invalid_index };
			size_t debug_shader_id{ invalid_index };
		};
	}
}