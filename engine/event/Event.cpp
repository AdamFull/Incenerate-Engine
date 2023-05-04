#include "Event.h"

using namespace engine;

EventId CEvent::getType() const
{
	return m_mType;
}

std::any CEvent::getValue(EventId id)
{
	return m_mData[id];
}

void CEvent::setValue(EventId id, const std::any& value)
{
	m_mData[id] = value;
}