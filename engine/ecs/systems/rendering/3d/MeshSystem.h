#pragma once

#include "ecs/systems/rendering/BaseGraphicsSystem.h"
#include "ecs/components/fwd.h"

namespace engine
{
	namespace ecs
	{
		class CMeshSystem : public CBaseGraphicsSystem
		{
		public:
			CMeshSystem() { name = "Rendering mesh system"; }
			virtual ~CMeshSystem() override = default;

			void __create() override;
			void __update(float fDt) override;

		private:
			void debugDrawSkeleton(entt::entity& node, glm::mat4 parentMatrix = glm::mat4(1.0f));
			void draw(const FCameraComponent* camera, graphics::EAlphaMode alphaMode);
		};
	}
}