#pragma once

#include "EngineStructures.h"
#include "ecs/components/fwd.h"

class btCollisionShape;

namespace engine
{
	namespace physics
	{
		class CPhysicsShape
		{
		public:
			CPhysicsShape(ecs::FPhysicsShapeComponent* collider);
			~CPhysicsShape();

			bool update(ecs::FPhysicsShapeComponent* collider);

			btCollisionShape* getShape() const;
			EPhysicsShapeType getShapeType() const;
		private:
			void createOrUpdateBoxShape(const glm::vec3& sizes);
			void createOrUpdateCapsuleShape(const float radius, const float height);
			void createOrUpdateConeShape(const float radius, const float height);
			void createOrUpdateCylinderShape(const glm::vec3& half_sizes);
			void createOrUpdateSphereShape(const float radius);

			void tryToDestroyShape();
		private:
			btCollisionShape* pShape{ nullptr };
			EPhysicsShapeType eType;
		};
	}
}