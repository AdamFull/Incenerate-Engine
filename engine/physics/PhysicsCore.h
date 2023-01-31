#pragma once

#include <bullet3/src/btBulletDynamicsCommon.h>
#include <bullet3/src/btBulletCollisionCommon.h>

namespace engine
{
	namespace physics
	{
		class CPhysicsCore
		{
		public:
			~CPhysicsCore();
			void create();
		private:
			btDefaultCollisionConfiguration* configuration{ nullptr };
			btCollisionDispatcher* dispatcher{ nullptr };
			btBroadphaseInterface* overlappingPairCache{ nullptr };
			btSequentialImpulseConstraintSolver* solver{ nullptr };
			btDiscreteDynamicsWorld* dynamicsWorld{ nullptr };
		};
	}
}