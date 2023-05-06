#pragma once

#include "EventInterface.h"

#include <memory>
#include <ufunction.hpp>

namespace engine
{
	class IEventManagerInterface
	{
	public:
		virtual ~IEventManagerInterface() = default;

		virtual std::unique_ptr<IEvent> makeEvent(EventId type) = 0;

		virtual void addListener(EventId eventId, utl::function<void(const std::unique_ptr<IEvent>&)> const& listener) = 0;

		virtual void sendEvent(const std::unique_ptr<IEvent>& evt) = 0;
		virtual void sendEvent(EventId eventId) = 0;
	};
}