#pragma once

#include "ecs/core/System.hpp"
#include "ecs/core/Event.hpp"

#include "system/window/KeycodeHelper.hpp"

namespace engine
{
	namespace ecs
	{
		class CInputSystem : public ISystem
		{
		public:
			virtual ~CInputSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		private:
			system::window::CKeyDecoder mKeys;
		private:
			void onKeyInput(CEvent& event);
			void onMouseInput(CEvent& event);
			void onAxisInput(CEvent& event);
		};
	}
}