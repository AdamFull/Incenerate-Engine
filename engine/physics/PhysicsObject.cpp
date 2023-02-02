#include "PhysicsObject.h"

#include "PhysicsHelper.h"

using namespace engine::physics;

CPhysicsObject::CPhysicsObject(btDynamicsWorld* world)
{
	pWorld = world;
}

CPhysicsObject::~CPhysicsObject()
{
	tryDestroyMotionState();
	tryDestroyRigidBody();
	tryDestroyCollider();
}

void CPhysicsObject::initialize(float fMass)
{
	this->fMass = fMass;

	pMotionState = new btDefaultMotionState();

	btVector3 localInertia(0, 0, 0);
	if (fMass != 0.f)
		pCollider->calculateLocalInertia(fMass, localInertia);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(fMass, pMotionState, pCollider, localInertia);
	pRigidBody = new btRigidBody(rbInfo);

	//add the body to the dynamics world
	pWorld->addRigidBody(pRigidBody);
}

void CPhysicsObject::clear()
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

void CPhysicsObject::setBoxCollider(const glm::vec3& sizes)
{
	eShapeType = EPhysicsShapeType::eBox;
	tryDestroyCollider();
	pCollider = new btBoxShape(vec3_to_btVector3(sizes));

	if (pRigidBody)
		pRigidBody->setCollisionShape(pCollider);
}

void CPhysicsObject::setCapsuleCollider(const float radius, const float height)
{
	eShapeType = EPhysicsShapeType::eCapsule;
	tryDestroyCollider();
	pCollider = new btCapsuleShape(radius, height);

	if (pRigidBody)
		pRigidBody->setCollisionShape(pCollider);
}

void CPhysicsObject::setConeCollider(const float radius, const float height)
{
	eShapeType = EPhysicsShapeType::eCone;
	tryDestroyCollider();
	pCollider = new btConeShape(radius, height);

	if (pRigidBody)
		pRigidBody->setCollisionShape(pCollider);
}

void CPhysicsObject::setCylinderCollider(const glm::vec3& half_sizes)
{
	eShapeType = EPhysicsShapeType::eCylinder;
	tryDestroyCollider();
	pCollider = new btCylinderShape(vec3_to_btVector3(half_sizes));

	if (pRigidBody)
		pRigidBody->setCollisionShape(pCollider);
}

void CPhysicsObject::setSphereCollider(const float radius)
{
	eShapeType = EPhysicsShapeType::eSphere;
	tryDestroyCollider();
	pCollider = new btSphereShape(radius);

	if (pRigidBody)
		pRigidBody->setCollisionShape(pCollider);
}

void CPhysicsObject::setBoxColliderSizes(const glm::vec3& sizes)
{
	if (pCollider && eShapeType == EPhysicsShapeType::eBox)
	{
		auto pBox = static_cast<btBoxShape*>(pCollider);

		auto boxHalfExtents = vec3_to_btVector3(sizes);
		btVector3 margin(pBox->getMargin(), pBox->getMargin(), pBox->getMargin());
		pBox->setImplicitShapeDimensions((boxHalfExtents * pBox->getLocalScaling()) - margin);
		pBox->setSafeMargin(boxHalfExtents);
	}
}

void CPhysicsObject::setCapsuleColliderRadiusAndHeight(const float radius, const float height)
{
	if (pCollider && eShapeType == EPhysicsShapeType::eCapsule)
	{
		auto pCapsule = static_cast<btCapsuleShape*>(pCollider);

		auto dims = btVector3(radius, 0.5f * height, radius);
		pCapsule->setImplicitShapeDimensions(dims);
	}
}

void CPhysicsObject::setConeColliderRadiusAndHeight(const float radius, const float height)
{
	if (pCollider && eShapeType == EPhysicsShapeType::eCone)
	{
		auto pCone = static_cast<btConeShape*>(pCollider);
	}
}

void CPhysicsObject::setCylinderColliderSizes(const glm::vec3& half_sizes)
{
	if (pCollider && eShapeType == EPhysicsShapeType::eCylinder)
	{
		auto pCylinder = static_cast<btCylinderShape*>(pCollider);
	}
}

void CPhysicsObject::setSphereColliderRadius(const float radius)
{
	if (pCollider && eShapeType == EPhysicsShapeType::eSphere)
	{
		auto pSphere = static_cast<btSphereShape*>(pCollider);
	}
}

void CPhysicsObject::setMass(const float mass)
{
	if (fMass != mass)
	{
		fMass = mass;

		btVector3 localInertia(0, 0, 0);
		if(fMass != 0.f)
			pCollider->calculateLocalInertia(fMass, localInertia);

		pRigidBody->setMassProps(fMass, localInertia);
	}
}

void CPhysicsObject::setWorldTranslation(const glm::mat4& translation)
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

glm::mat4 CPhysicsObject::getWorldTranslation()
{
	btTransform btTranslation;

	//if (pMotionState)
	//	pMotionState->getWorldTransform(btTranslation);
	//else
		btTranslation = pRigidBody->getWorldTransform();

	return btTransform_to_mat4(btTranslation);
}

void CPhysicsObject::tryDestroyMotionState()
{
	if (pMotionState)
	{
		delete pMotionState;
		pMotionState = nullptr;
	}

	if (pRigidBody)
		pRigidBody->setMotionState(nullptr);
}

void CPhysicsObject::tryDestroyRigidBody()
{
	if (pRigidBody)
	{
		pWorld->removeRigidBody(pRigidBody);

		delete pRigidBody;
		pRigidBody = nullptr;
	}
}

void CPhysicsObject::tryDestroyCollider()
{
	if (pCollider)
	{
		delete pCollider;
		pCollider = nullptr;
	}
}