#include "Engine.h"

#include "PhysicsHelper.h"
#include "PhysicsDebugDraw.h"

using namespace engine::physics;

CPhysicsCore::~CPhysicsCore()
{
	pPhysicsObjectManager = nullptr;

	if (debugDraw)
	{
		dynamicsWorld->setDebugDrawer(nullptr);

		delete debugDraw;
		debugDraw = nullptr;
	}

	if (dynamicsWorld)
	{
		delete dynamicsWorld;
		dynamicsWorld = nullptr;
	}

	if (solver)
	{
		delete solver;
		solver = nullptr;
	}

	if (overlappingPairCache)
	{
		delete overlappingPairCache;
		overlappingPairCache = nullptr;
	}
	
	if (dispatcher)
	{
		delete dispatcher;
		dispatcher = nullptr;
	}

	if (configuration)
	{
		delete configuration;
		configuration = nullptr;
	}
}

void CPhysicsCore::create()
{
	pPhysicsObjectManager = std::make_unique<CObjectManager<CPhysicsObject>>();

	configuration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(configuration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, configuration);

	//ghostPairCB = new btGhostPairCallback();

	// if physics debug
	{
		debugDraw = new CPhysicsDebugDraw();
		dynamicsWorld->setDebugDrawer(debugDraw);
	}

	setGravity(glm::vec3(0.f, -9.8f, 0.f));
}

void CPhysicsCore::simulate(float fDT)
{
	pPhysicsObjectManager->performDeletion();

	if (dynamicsWorld->getDebugDrawer() && EGEngine->isDebugDrawPhysics())
		dynamicsWorld->debugDrawWorld();

	dynamicsWorld->stepSimulation(fDT, 10);
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

void CPhysicsCore::reset()
{
	dynamicsWorld->clearForces();

	auto& objects = pPhysicsObjectManager->getAll();
	for (auto& [id, object] : objects)
		object->clear();
}

void CPhysicsCore::setDebugDrawEnable(bool enable)
{
	if (enable && !debugDraw)
		debugDraw = new CPhysicsDebugDraw();

	if (enable)
		dynamicsWorld->setDebugDrawer(debugDraw);
	else
		dynamicsWorld->setDebugDrawer(nullptr);
}

size_t CPhysicsCore::addRigidBody(const std::string& name)
{
	return pPhysicsObjectManager->add(name, std::make_unique<CRigidBodyObject>(dynamicsWorld));
}

size_t CPhysicsCore::addRigidBody(const std::string& name, std::unique_ptr<CRigidBodyObject>&& source)
{
	return pPhysicsObjectManager->add(name, std::move(source));
}

size_t CPhysicsCore::addCollider(const std::string& name)
{
	return pPhysicsObjectManager->add(name, std::make_unique<CColliderObject>(dynamicsWorld));
}

size_t CPhysicsCore::addCollider(const std::string& name, std::unique_ptr<CColliderObject>&& source)
{
	return pPhysicsObjectManager->add(name, std::move(source));
}

void CPhysicsCore::removeObject(const std::string& name)
{
	pPhysicsObjectManager->remove(name);
}

void CPhysicsCore::removeObject(size_t id)
{
	pPhysicsObjectManager->remove(id);
}

const std::unique_ptr<CPhysicsObject>& CPhysicsCore::getObject(const std::string& name)
{
	return pPhysicsObjectManager->get(name);
}

const std::unique_ptr<CPhysicsObject>& CPhysicsCore::getObject(size_t id)
{
	return pPhysicsObjectManager->get(id);
}
