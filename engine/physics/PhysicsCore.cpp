#include "PhysicsCore.h"

#include <bullet3/src/btBulletDynamicsCommon.h>
#include <bullet3/src/btBulletCollisionCommon.h>

#include "PhysicsHelper.h"

using namespace engine::physics;

CPhysicsCore::~CPhysicsCore()
{
	delete dynamicsWorld;
	dynamicsWorld = nullptr;

	delete solver;
	solver = nullptr;

	delete overlappingPairCache;
	overlappingPairCache = nullptr;

	delete dispatcher;
	dispatcher = nullptr;

	delete configuration;
	configuration = nullptr;
}

void CPhysicsCore::create()
{
	pPhysicsObjectManager = std::make_unique<CObjectManager<CPhysicsObject>>();

	configuration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(configuration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, configuration);

	setGravity(glm::vec3(0.f, -9.8f, 0.f));
}

void CPhysicsCore::simulate(float fDT)
{
	dynamicsWorld->stepSimulation(fDT, 3);
}

bool CPhysicsCore::rayCast(const glm::vec3& startPos, const glm::vec3& endPos, FHitResult* hitResult)
{
	auto btStart = vec3_to_btVector3(startPos);
	auto btEnd = vec3_to_btVector3(endPos);
	btCollisionWorld::ClosestRayResultCallback btResult(btStart, btEnd);
	dynamicsWorld->rayTest(btStart, btEnd, btResult);

	if (btResult.hasHit())
	{
		//result->hitObj = getPhysicsObject(btResult.m_collisionObject);
		//result->hitPosition = convertbtVector3ToVec3(btResult.m_hitPointWorld);
		//result->hitNormal = convertbtVector3ToVec3(btResult.m_hitNormalWorld);
		return true;
	}

	return false;
}

void CPhysicsCore::setGravity(const glm::vec3& gravity)
{
	dynamicsWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}