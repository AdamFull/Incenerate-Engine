#pragma once

#include "ObjectManager.hpp"
#include "PhysicsObject.h"

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

			void reset();

			size_t addObject(const std::string& name);
			size_t addObject(const std::string& name, std::unique_ptr<CPhysicsObject>&& source);
			void removeObject(const std::string& name);
			void removeObject(size_t id);
			const std::unique_ptr<CPhysicsObject>& getObject(const std::string& name);
			const std::unique_ptr<CPhysicsObject>& getObject(size_t id);

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