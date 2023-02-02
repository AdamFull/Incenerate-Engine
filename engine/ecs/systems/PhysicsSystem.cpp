#include "PhysicsSystem.h"

#include "Engine.h"

#include "ecs/components/RigidBodyComponent.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/HierarchyComponent.h"
#include "ecs/systems/HierarchySystem.h"

using namespace engine::ecs;

void CPhysicsSystem::__create()
{

}

void CPhysicsSystem::__update(float fDt)
{
	auto& physics = EGEngine->getPhysics();

	if (!EGEngine->isEditorEditing())
	{
		// Synchronize scene with physics
		{
			auto view = registry->view<FTransformComponent, FRigidBodyComponent>();
			for (auto [entity, transform, rigidbody] : view.each())
			{
				auto& object = physics->getObject(rigidbody.object_id);

				object->setMass(rigidbody.mass);
				object->setWorldTranslation(transform.model);

				switch (rigidbody.type)
				{
				case EPhysicsShapeType::eBox: { object->setBoxColliderSizes(rigidbody.sizes); } break;
				case EPhysicsShapeType::eCapsule: {} break;
				case EPhysicsShapeType::eCone: {} break;
				case EPhysicsShapeType::eCylinder: {} break;
				case EPhysicsShapeType::eSphere: {} break;
				}
			}
		}

		physics->simulate(fDt);

		// Synchronize physics with scene
		{
			auto view = registry->view<FTransformComponent, FRigidBodyComponent>();
			for (auto [entity, transform, rigidbody] : view.each())
			{
				auto& object = physics->getObject(rigidbody.object_id);
				transform.model = object->getWorldTranslation();
				transform.update();

				auto delta = transform.model - transform.model_old + glm::mat4(1.f);
				transform.apply_delta(delta);

				CHierarchySystem::initialize_matrices(registry, entity);
				CHierarchySystem::calculate_matrices(registry, entity);
			}
		}
	}
}