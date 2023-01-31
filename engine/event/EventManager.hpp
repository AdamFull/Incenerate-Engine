#pragma once

#include "Event.hpp"

#include <list>

namespace engine
{
	class CEventManager
	{
	public:
		void addListener(EventId eventId, utl::function<void(CEvent&)> const& listener);

		void sendEvent(CEvent& event);
		void sendEvent(EventId eventId);

	private:
		std::unordered_map<EventId, std::vector<utl::function<void(CEvent&)>>> listeners;
	};
}