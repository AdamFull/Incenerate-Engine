#pragma once

#include "ObjectManager.hpp"
#include "PhysicsObject.h"

class btDynamicsWorld;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
struct btDbvtBroadphase;
class btSequentialImpulseConstraintSolver;
class btGhostPairCallback;
class btRigidBody;
class btCollisionObject;
class btBroadphaseInterface;
class btDiscreteDynamicsWorld;

namespace engine
{
	namespace physics
	{
		struct FHitResult
		{
			glm::vec3 hitPosition;
			glm::vec3 hitNormal;
			size_t hit_id{ invalid_index };
		};

		class CPhysicsCore
		{
		public:
			~CPhysicsCore();
			void create();
			void simulate(float fDT);

			bool rayCast(const glm::vec3& startPos, const glm::vec3& endPos, FHitResult* hitResult);

			void setGravity(const glm::vec3& gravity);
		private:
			std::unique_ptr<CObjectManager<CPhysicsObject>> pPhysicsObjectManager;

			btDefaultCollisionConfiguration* configuration{ nullptr };
			btCollisionDispatcher* dispatcher{ nullptr };
			btBroadphaseInterface* overlappingPairCache{ nullptr };
			btSequentialImpulseConstraintSolver* solver{ nullptr };
			btDiscreteDynamicsWorld* dynamicsWorld{ nullptr };
		};
	}
}