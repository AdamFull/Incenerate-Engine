#include "PhysicsObject.h"

#include "PhysicsHelper.h"
#include <bullet3/src/BulletCollision/CollisionDispatch/btGhostObject.h>

#include "ecs/components/RigidBodyComponent.h"

using namespace engine::ecs;
using namespace engine::physics;

// Base class
CPhysicsObject::~CPhysicsObject()
{
	if (pCollisionShape)
	{
		delete pCollisionShape;
		pCollisionShape = nullptr;
	}
}

CPhysicsShape* CPhysicsObject::getCollisionShape() const
{
	return pCollisionShape;
}

// Rigidbody
CRigidBodyObject::CRigidBodyObject(btDynamicsWorld* world)
{
	pWorld = world;
}

CRigidBodyObject::~CRigidBodyObject()
{
	tryDestroyMotionState();
	tryDestroyRigidBody();
}

void CRigidBodyObject::initialize(void* comp)
{
	auto* rigidbody = static_cast<FRigidBodyComponent*>(comp);

	fMass = rigidbody->mass;

	pCollisionShape = new CPhysicsShape(&rigidbody->shape);
	auto pShape = pCollisionShape->getShape();

	pMotionState = new btDefaultMotionState();

	btVector3 localInertia(0, 0, 0);
	if (fMass != 0.f)
		pShape->calculateLocalInertia(fMass, localInertia);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(fMass, pMotionState, pShape, localInertia);
	pRigidBody = new btRigidBody(rbInfo);

	pRigidBody->setActivationState(DISABLE_DEACTIVATION);

	//add the body to the dynamics world
	pWorld->addRigidBody(pRigidBody);
}

void CRigidBodyObject::update(void* comp)
{
	auto* rigidbody = static_cast<FRigidBodyComponent*>(comp);

	// Updating shape if needed
	if (pCollisionShape->update(&rigidbody->shape))
	{
		auto pShape = pCollisionShape->getShape();
		pRigidBody->setCollisionShape(pShape);
	}

	// Updating mass
	if (fMass != rigidbody->mass)
	{
		fMass = rigidbody->mass;

		btVector3 localInertia(0, 0, 0);
		if (fMass != 0.f)
		{
			auto pShape = pCollisionShape->getShape();
			pShape->calculateLocalInertia(fMass, localInertia);
		}

		pRigidBody->setMassProps(fMass, localInertia);
	}
}

void CRigidBodyObject::clear()
{
	pWorld->removeRigidBody(pRigidBody);

	pRigidBody->clearForces();
	pRigidBody->clearGravity();

	btVector3 zeroVector(0, 0, 0);
	pRigidBody->setLinearVelocity(zeroVector);
	pRigidBody->setAngularVelocity(zeroVector);
	pRigidBody->activate();

	pWorld->addRigidBody(pRigidBody);
}

void CRigidBodyObject::setWorldTranslation(const glm::mat4& translation)
{
	auto btTranslate = mat4_to_btTransform(translation);

	if (pMotionState)
	{
		pMotionState->setWorldTransform(btTranslate);

		if (pRigidBody)
			pRigidBody->setMotionState(pMotionState);
	}
	else
		pRigidBody->setWorldTransform(btTranslate);
}

glm::mat4 CRigidBodyObject::getWorldTranslation()
{
	btTransform btTranslation;

	//if (pMotionState)
	//	pMotionState->getWorldTransform(btTranslation);
	//else
	btTranslation = pRigidBody->getWorldTransform();

	return btTransform_to_mat4(btTranslation);
}

const bool CRigidBodyObject::isActive() const
{
	return pRigidBody->isActive();
}

const bool CRigidBodyObject::isStatic() const
{
	return pRigidBody->isStaticObject();
}

const bool CRigidBodyObject::isKinematic() const
{
	return pRigidBody->isKinematicObject();
}

const bool CRigidBodyObject::isDynamic() const
{
	return fMass != 0.f;
}

void CRigidBodyObject::tryDestroyMotionState()
{
	if (pMotionState)
	{
		delete pMotionState;
		pMotionState = nullptr;
	}

	if (pRigidBody)
		pRigidBody->setMotionState(nullptr);
}

void CRigidBodyObject::tryDestroyRigidBody()
{
	if (pRigidBody)
	{
		pRigidBody->setCollisionShape(nullptr);
		pWorld->removeRigidBody(pRigidBody);

		delete pRigidBody;
		pRigidBody = nullptr;
	}
}


CColliderObject::CColliderObject(btDynamicsWorld* world)
{
	pWorld = world;
}

CColliderObject::~CColliderObject()
{
	if (pGhost)
	{
		delete pGhost;
		pGhost = nullptr;
	}
}

void CColliderObject::initialize(void* rigidbody)
{
}

void CColliderObject::update(void* comp)
{
}

void CColliderObject::clear()
{
}

void CColliderObject::setWorldTranslation(const glm::mat4& translation)
{
	pGhost->setWorldTransform(mat4_to_btTransform(translation));
}

glm::mat4 CColliderObject::getWorldTranslation()
{
	return btTransform_to_mat4(pGhost->getWorldTransform());
}