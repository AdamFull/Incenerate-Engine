#pragma once

#include "EngineStructures.h"

class btCollisionShape;
class btRigidBody;
class btDefaultMotionState;
class btDynamicsWorld;

namespace engine
{
	namespace physics
	{
		class CPhysicsObject
		{
		public:
			CPhysicsObject(btDynamicsWorld* world);
			~CPhysicsObject();

			void initialize(float fMass);
			void clear();

			void setBoxCollider(const glm::vec3& sizes);
			void setCapsuleCollider(const float radius, const float height);
			void setConeCollider(const float radius, const float height);
			void setCylinderCollider(const glm::vec3& half_sizes);
			void setSphereCollider(const float radius);

			void setBoxColliderSizes(const glm::vec3& sizes);
			void setCapsuleColliderRadiusAndHeight(const float radius, const float height);
			void setConeColliderRadiusAndHeight(const float radius, const float height);
			void setCylinderColliderSizes(const glm::vec3& half_sizes);
			void setSphereColliderRadius(const float radius);

			void setMass(const float mass);

			void setWorldTranslation(const glm::mat4& translation);
			glm::mat4 getWorldTranslation();

			const bool isActive() const;

			EPhysicsShapeType getShapeType() const { return eShapeType; }

		private:
			void tryDestroyMotionState();
			void tryDestroyRigidBody();
			void tryDestroyCollider();
		private:
			float fMass{ 0.f };
			EPhysicsShapeType eShapeType{ EPhysicsShapeType::eBox };

			btCollisionShape* pCollider{ nullptr };
			btDefaultMotionState* pMotionState{ nullptr };
			btRigidBody* pRigidBody{ nullptr };
			btDynamicsWorld* pWorld{ nullptr };
		};
	}
}