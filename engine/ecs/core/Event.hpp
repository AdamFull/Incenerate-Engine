#pragma once

#include "Type.hpp"
#include <any>
#include <unordered_map>

namespace engine
{
	namespace ecs
	{
		class CEvent
		{
		public:
			CEvent() = delete;

			explicit CEvent(EventId type) : mType(type) {}

			template<class _Ty>
			void setParam(EventId id, _Ty value)
			{
				mData[id] = value;
			}

			template<class _Ty>
			_Ty getParam(EventId id)
			{
				return std::any_cast<_Ty>(mData[id]);
			}

			EventId getType() const
			{
				return mType;
			}
		private:
			EventId mType{};
			std::unordered_map<EventId, std::any> mData{};
		};
	}
}