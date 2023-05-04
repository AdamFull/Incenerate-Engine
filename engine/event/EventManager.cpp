#include "EventManager.hpp"

#include "Event.h"

using namespace engine;

std::unique_ptr<IEvent> CEventManager::makeEvent(EventId type)
{
	return std::make_unique<CEvent>(type);
}

void CEventManager::addListener(EventId eventId, utl::function<void(const std::unique_ptr<IEvent>&)> const& listener)
{
	listeners[eventId].push_back(listener);
}

void CEventManager::sendEvent(const std::unique_ptr<IEvent>& event)
{
	uint32_t type = event->getType();

	for (auto& listener : listeners[type])
		listener(event);
}

void CEventManager::sendEvent(EventId eventId)
{
	auto event = makeEvent(eventId);

	for (auto& listener : listeners[eventId])
		listener(event);
}