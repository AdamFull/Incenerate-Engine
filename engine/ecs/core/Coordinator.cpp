#include "Coordinator.hpp"

using namespace engine::ecs;

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
void CCoordinator::addEventListener(EventId eventId, utl::function<void(CEvent&)> const& listener)
{
	pEventManager->addListener(eventId, listener);
}

void CCoordinator::sendEvent(CEvent& event)
{
	pEventManager->sendEvent(event);
}

void CCoordinator::sendEvent(EventId eventId)
{
	pEventManager->sendEvent(eventId);
}