#include "Coordinator.hpp"

using namespace engine::ecs;

// Based on: https://austinmorlan.com/posts/entity_component_system/
void CCoordinator::create()
{
	pComponentManager = std::make_unique<CComponentManager>();
	pEntityManager = std::make_unique<CEntityManager>();
	pEventManager = std::make_unique<CEventManager>();
	pSystemManager = std::make_unique<CSystemManager>();
}

// Entity methods
Entity CCoordinator::createEntity()
{
	return pEntityManager->create();
}

void CCoordinator::destroyEntity(Entity entity)
{
	pEntityManager->destroy(entity);
	pComponentManager->entityDestroyed(entity);
	pSystemManager->entityDestroyed(entity);
}

// Event methods
void CCoordinator::sendEvent(CEvent& event)
{
	pEventManager->sendEvent(event);
}

void CCoordinator::sendEvent(EventId eventId)
{
	pEventManager->sendEvent(eventId);
}

void CCoordinator::setActiveCamera(Entity entity)
{
	activeCamera = entity;
}

const std::optional<Entity>& CCoordinator::getActiveCamera() const
{
	return activeCamera;
}

void CCoordinator::setActiveEnvironment(Entity entity)
{
	activeEnvironment = entity;
}

const std::optional<Entity>& CCoordinator::getActiveEnvironment() const
{
	return activeEnvironment;
}