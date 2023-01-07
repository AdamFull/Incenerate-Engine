#pragma once

#include <any>
#include <unordered_map>

namespace engine
{
	// Source: https://gist.github.com/Lee-R/3839813
	constexpr std::uint32_t fnv1a_32(char const* s, std::size_t count)
	{
		return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u; // NOLINT (hicpp-signed-bitwise)
	}

	constexpr std::uint32_t operator "" _hash(char const* s, std::size_t count)
	{
		return fnv1a_32(s, count);
	}

	// Events
	using EventId = uint32_t;
	using ParamId = uint32_t;

	namespace Events::Graphics
	{
		const ParamId ReCreate = "Events::Graphics::ReCreate"_hash;
	}

	namespace Events::Input
	{
		const ParamId Key = "Events::Input::Key"_hash;
		const ParamId Mouse = "Events::Input::Mouse"_hash;
		const ParamId MouseX = "Events::Input::MouseX"_hash;
		const ParamId MouseY = "Events::Input::MouseY"_hash;
		const ParamId Axis = "Events::Input::Axis"_hash;
	}

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