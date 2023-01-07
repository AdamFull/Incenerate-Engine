#pragma once

#include "Event.hpp"

#include <utility/ufunction.hpp>
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
		std::unordered_map<EventId, std::list<utl::function<void(CEvent&)>>> listeners;
	};
}