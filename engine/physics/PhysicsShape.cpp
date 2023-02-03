#include "PhysicsShape.h"

#include "ecs/components/RigidBodyComponent.h"

#include "PhysicsHelper.h"

using namespace engine;
using namespace engine::ecs;
using namespace engine::physics;

CPhysicsShape::CPhysicsShape(FPhysicsShapeComponent* collider)
{
	update(collider);
}

CPhysicsShape::~CPhysicsShape()
{
	tryToDestroyShape();
}

bool CPhysicsShape::update(FPhysicsShapeComponent* collider)
{
	bool updated{ false };
	auto ctype = static_cast<EPhysicsShapeType>(collider->type);

	if (eType != ctype)
	{
		eType = ctype;
		tryToDestroyShape();
		updated = true;
	}

	switch (eType)
	{
	case engine::EPhysicsShapeType::eBox: createOrUpdateBoxShape(collider->sizes); break;
	case engine::EPhysicsShapeType::eCapsule: createOrUpdateCapsuleShape(collider->radius, collider->height); break;
	case engine::EPhysicsShapeType::eCone: createOrUpdateConeShape(collider->radius, collider->height); break;
	case engine::EPhysicsShapeType::eCylinder: createOrUpdateCylinderShape(collider->sizes); break;
	case engine::EPhysicsShapeType::eSphere: createOrUpdateSphereShape(collider->radius);
	}

	return updated;
}

void CPhysicsShape::tryToDestroyShape()
{
	if (pShape)
	{
		delete pShape;
		pShape = nullptr;
	}
}

btCollisionShape* CPhysicsShape::getShape() const
{
	return pShape;
}

EPhysicsShapeType CPhysicsShape::getShapeType() const
{
	return eType;
}


// TODO: add checking is update needed
void CPhysicsShape::createOrUpdateBoxShape(const glm::vec3& sizes)
{
	if (!pShape)
		pShape = new btBoxShape(vec3_to_btVector3(sizes));
	else
	{
		auto pBox = static_cast<btBoxShape*>(pShape);

		auto boxHalfExtents = vec3_to_btVector3(sizes);
		btVector3 margin(pBox->getMargin(), pBox->getMargin(), pBox->getMargin());
		pBox->setImplicitShapeDimensions((boxHalfExtents * pBox->getLocalScaling()) - margin);
		pBox->setSafeMargin(boxHalfExtents);
	}
}

void CPhysicsShape::createOrUpdateCapsuleShape(const float radius, const float height)
{
	if (!pShape)
		pShape = new btCapsuleShape(radius, height);
	else
	{
		auto pCapsule = static_cast<btCapsuleShape*>(pShape);

		auto dims = btVector3(radius, 0.5f * height, radius);
		pCapsule->setImplicitShapeDimensions(dims);
	}
}

void CPhysicsShape::createOrUpdateConeShape(const float radius, const float height)
{
	if (!pShape)
		pShape = new btConeShape(radius, height);
	else
	{
		// TODO: add cone shape update
		auto pCone = static_cast<btConeShape*>(pShape);

		pCone->setRadius(radius);
		pCone->setHeight(height);
		pCone->setConeUpIndex(1);
		pCone->setLocalScaling(btVector3(1.f, 1.f, 1.f));
	}
}

void CPhysicsShape::createOrUpdateCylinderShape(const glm::vec3& half_sizes)
{
	if (!pShape)
		pShape = new btCylinderShape(vec3_to_btVector3(half_sizes));
	else
	{
		auto pCylinder = static_cast<btCylinderShape*>(pShape);

		auto cylinderalfExtents = vec3_to_btVector3(half_sizes);
		btVector3 margin(pCylinder->getMargin(), pCylinder->getMargin(), pCylinder->getMargin());
		pCylinder->setImplicitShapeDimensions((cylinderalfExtents * pCylinder->getLocalScaling()) - margin);
		pCylinder->setSafeMargin(cylinderalfExtents);
	}
}

void CPhysicsShape::createOrUpdateSphereShape(const float radius)
{
	if (!pShape)
		pShape = new btSphereShape(radius);
	else
	{
		auto pSphere = static_cast<btSphereShape*>(pShape);
		pSphere->setMargin(radius);
	}
}