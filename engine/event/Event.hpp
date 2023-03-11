#pragma once

#include <any>
#include <unordered_map>

namespace engine
{
	// Events
	using EventId = uint32_t;
	using ParamId = uint32_t;

	namespace Events::Debug
	{
		using namespace utl;
		constexpr const inline ParamId SystemCreateTime = "Events::Debug::SystemCreateTime"_utl_hash;
		constexpr const inline ParamId SystemUpdateTime = "Events::Debug::SystemUpdateTime"_utl_hash;
	}

	namespace Events::Editor
	{
		using namespace utl;
		constexpr const inline ParamId OpenImageViewer = "Events::Editor::OpenImageViewer"_utl_hash;
		constexpr const inline ParamId SelectMaterial = "Events::Editor::SelectMaterial"_utl_hash;
		constexpr const inline ParamId ProjectUpdated = "Events::Editor::ProjectUpdated"_utl_hash;
	}

	namespace Events::Graphics
	{
		using namespace utl;
		constexpr const inline ParamId ReCreate = "Events::Graphics::ReCreate"_utl_hash;
		constexpr const inline ParamId ViewportReCreate = "Events::Graphics::ViewportReCreate"_utl_hash;
		constexpr const inline ParamId AllFramesDone = "Events::Graphics::AllFramesDone"_utl_hash;
	}

	namespace Events::Input
	{
		using namespace utl;
		constexpr const inline ParamId Key = "Events::Input::Key"_utl_hash;
		constexpr const inline ParamId Mouse = "Events::Input::Mouse"_utl_hash;
		constexpr const inline ParamId MouseX = "Events::Input::MouseX"_utl_hash;
		constexpr const inline ParamId MouseY = "Events::Input::MouseY"_utl_hash;
		constexpr const inline ParamId Axis = "Events::Input::Axis"_utl_hash;
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