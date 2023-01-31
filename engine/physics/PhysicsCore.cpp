#include "PhysicsCore.h"

using namespace engine::physics;

CPhysicsCore::~CPhysicsCore()
{
	delete dynamicsWorld;
	delete solver;
	delete overlappingPairCache;
	delete dispatcher;
	delete configuration;
}

void CPhysicsCore::create()
{
	configuration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(configuration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, configuration);

	setGravity(glm::vec3(0.f, -9.8f, 0.f));
}

void CPhysicsCore::setGravity(const glm::vec3& gravity)
{
	dynamicsWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}