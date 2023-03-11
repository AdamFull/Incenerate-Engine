#pragma once

#include "WindowBase.h"
#include "event/Event.hpp"

namespace engine
{
	namespace editor
	{
		class CMaterialEditorWindow : public IEditorWindow
		{
		public:
			CMaterialEditorWindow(const std::string& sname) { name = sname; }
			virtual ~CMaterialEditorWindow() override = default;

			void create() override;
			void __draw(float fDt) override;
		private:
			void onSetMaterial(CEvent& event);
		private:
			size_t selected_material{ invalid_index };
		};
	}
}