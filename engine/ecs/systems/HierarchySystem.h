#pragma once

#include "BaseSystem.h"
#include "ecs/components/fwd.h"

namespace engine
{
	namespace ecs
	{
		class CHierarchySystem : public ISystem
		{
		public:
			friend class CPhysicsSystem;

			CHierarchySystem() { name = "Hierarchy system"; }
			virtual ~CHierarchySystem() override = default;

			void __create() override;
			void __update(float fDt) override;

		private:
			static void initialize_matrix(FTransformComponent* transform);
			static void initialize_matrices(entt::registry* registry, entt::entity node);
			static void calculate_matrices(entt::registry* registry, entt::entity node);
		};
	}
}