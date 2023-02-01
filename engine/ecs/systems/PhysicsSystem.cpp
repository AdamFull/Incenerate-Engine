#include "PhysicsSystem.h"

#include "Engine.h"

using namespace engine::ecs;

void CPhysicsSystem::__create()
{

}

void CPhysicsSystem::__update(float fDt)
{
	auto& physics = EGEngine->getPhysics();

	physics->simulate(fDt);
}