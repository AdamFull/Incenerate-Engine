#include "EventManager.hpp"

using namespace engine::ecs;

void CEventManager::addListener(EventId eventId, utl::function<void(CEvent&)> const& listener)
{
	listeners[eventId].push_back(listener);
}

void CEventManager::sendEvent(CEvent& event)
{
	uint32_t type = event.getType();

	for (auto& listener : listeners[type])
		listener(event);
}

void CEventManager::sendEvent(EventId eventId)
{
	CEvent event(eventId);

	for (auto& listener : listeners[eventId])
		listener(event);
}