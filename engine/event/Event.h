#pragma once

#include <ie_event_exports.h>

#include <interface/event/EventInterface.h>

#include <unordered_map>

namespace engine
{
	class INCENERATE_EVENT_EXPORT CEvent final : public IEvent
	{
	public:
		CEvent() = delete;

		explicit CEvent(EventId type) : m_mType(type) {}

		EventId getType() const override;
	protected:
		std::any getValue(EventId id) override;
		void setValue(EventId id, const std::any& value) override;
	private:
		EventId m_mType{};
		std::unordered_map<EventId, std::any> m_mData{};
	};
}