#pragma once

#include "BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CSkinningSystem : public ISystem
		{
		public:
			CSkinningSystem() { name = "Skinning system"; }

			void __create() override;
			void __update(float fDt) override;
		private:
			void debugDrawSkeleton(entt::entity& node, glm::mat4 parentMatrix = glm::mat4(1.f));
		};
	}
}