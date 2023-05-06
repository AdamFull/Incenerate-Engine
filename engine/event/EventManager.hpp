#pragma once

#include <interface/event/EventManagerInterface.h>

namespace engine
{
	class CEventManager : public IEventManagerInterface
	{
	public:
		std::unique_ptr<IEvent> makeEvent(EventId type) override;

		void addListener(EventId eventId, utl::function<void(const std::unique_ptr<IEvent>&)> const& listener) override;

		void sendEvent(const std::unique_ptr<IEvent>& event) override;
		void sendEvent(EventId eventId) override;

	private:
		std::unordered_map<EventId, std::vector<utl::function<void(const std::unique_ptr<IEvent>&)>>> listeners;
	};
}