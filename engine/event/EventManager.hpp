#pragma once

#include "EventManagerInterface.h"

namespace engine
{
	class CEventManager : public IEventManagerInterface
	{
	public:
		void addListener(EventId eventId, utl::function<void(CEvent&)> const& listener) override;

		void sendEvent(CEvent& event) override;
		void sendEvent(EventId eventId) override;

	private:
		std::unordered_map<EventId, std::vector<utl::function<void(CEvent&)>>> listeners;
	};
}