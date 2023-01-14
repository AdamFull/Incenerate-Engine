#pragma once

#include <utility/upattern.hpp>
#include <any>
#include <unordered_map>

namespace engine
{
	// Events
	using EventId = uint32_t;
	using ParamId = uint32_t;

	namespace Events::Editor
	{
		using namespace utl;
		const ParamId OpenImageViewer = "Events::Editor::OpenImageViewer"_utl_hash;
	}

	namespace Events::Graphics
	{
		using namespace utl;
		const ParamId ReCreate = "Events::Graphics::ReCreate"_utl_hash;
		const ParamId ViewportReCreate = "Events::Graphics::ViewportReCreate"_utl_hash;
		const ParamId PerformDelete = "Events::Graphics::PerformDelete"_utl_hash;
	}

	namespace Events::Input
	{
		using namespace utl;
		const ParamId Key = "Events::Input::Key"_utl_hash;
		const ParamId Mouse = "Events::Input::Mouse"_utl_hash;
		const ParamId MouseX = "Events::Input::MouseX"_utl_hash;
		const ParamId MouseY = "Events::Input::MouseY"_utl_hash;
		const ParamId Axis = "Events::Input::Axis"_utl_hash;
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