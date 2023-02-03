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

				object->update(&rigidbody);
				object->setWorldTranslation(transform.model);
			}
		}

		physics->simulate(fDt);

		// Synchronize physics with scene
		{
			auto view = registry->view<FTransformComponent, FRigidBodyComponent>();
			for (auto [entity, transform, rigidbody] : view.each())
			{
				auto& object = physics->getObject(rigidbody.object_id);

				auto nmodel = object->getWorldTranslation();
				auto delta = nmodel - transform.model + glm::mat4(1.f);
				transform.apply_delta(delta);

				transform.model = nmodel;
				transform.update();

				CHierarchySystem::initialize_matrices(registry, entity);
				CHierarchySystem::calculate_matrices(registry, entity);
			}
		}
	}
}