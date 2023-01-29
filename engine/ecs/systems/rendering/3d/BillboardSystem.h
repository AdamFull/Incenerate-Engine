#pragma once

#include "ecs/systems/rendering/BaseGraphicsSystem.h"
#include "ecs/components/fwd.h"

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
			void draw(FCameraComponent* camera, const glm::vec3& position, const glm::vec3& color, entt::entity entity);
			size_t shader_id{ invalid_index };
			size_t vbo_id{ invalid_index };

			size_t audio_icon{ invalid_index };
			size_t camera_icon{ invalid_index };
			size_t environment_icon{ invalid_index };
			size_t light_icon{ invalid_index };
		};
	}
}