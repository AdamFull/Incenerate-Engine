#pragma once

#include "PhysicsShape.h"

class btRigidBody;
class btDefaultMotionState;
class btGhostObject;
class btDynamicsWorld;

namespace engine
{
	namespace physics
	{
		class CPhysicsObject
		{
		public:
			virtual ~CPhysicsObject();

			virtual void initialize(void* comp) { comp; }
			virtual void update(void* comp) { comp; }
			virtual void clear() {}

			virtual void setWorldTranslation(const glm::mat4& translation) { translation; }
			virtual glm::mat4 getWorldTranslation() { return glm::mat4(); }

			CPhysicsShape* getCollisionShape() const;
		protected:
			CPhysicsShape* pCollisionShape{ nullptr };
			btDynamicsWorld* pWorld{ nullptr };
		};

		class CRigidBodyObject : public CPhysicsObject
		{
		public:
			CRigidBodyObject(btDynamicsWorld* world);
			virtual ~CRigidBodyObject() override;

			void initialize(void* rigidbody) override;
			void update(void* comp) override;
			void clear() override;

			void setWorldTranslation(const glm::mat4& translation) override;
			glm::mat4 getWorldTranslation() override;

			const bool isActive() const;
			const bool isStatic() const;
			const bool isKinematic() const;
			const bool isDynamic() const;
		private:
			void tryDestroyMotionState();
			void tryDestroyRigidBody();
		private:
			float fMass{ 0.f };
			btDefaultMotionState* pMotionState{ nullptr };
			btRigidBody* pRigidBody{ nullptr };
		};


		class CColliderObject : public CPhysicsObject
		{
		public:
			CColliderObject(btDynamicsWorld* world);
			virtual ~CColliderObject() override;

			void initialize(void* rigidbody) override;
			void update(void* comp) override;
			void clear() override;

			void setWorldTranslation(const glm::mat4& translation) override;
			glm::mat4 getWorldTranslation() override;
		private:
			btGhostObject* pGhost{ nullptr };
		};
	}
}