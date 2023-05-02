#pragma once

#include "Event.hpp"

namespace engine
{
	class IEventManagerInterface
	{
	public:
		virtual ~IEventManagerInterface() = default;
		virtual void addListener(EventId eventId, utl::function<void(CEvent&)> const& listener) = 0;

		virtual void sendEvent(CEvent& evt) = 0;
		virtual void sendEvent(EventId eventId) = 0;
	};
}