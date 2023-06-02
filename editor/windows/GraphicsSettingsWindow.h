#pragma once

#include "WindowBase.h"

namespace engine
{
	namespace editor
	{
		class CGraphicsSettingsWindow : public IEditorWindow
		{
		public:
			CGraphicsSettingsWindow(const std::string& sname) { name = sname; }
			virtual ~CGraphicsSettingsWindow() override = default;

			void create() override;
			void __draw(float fDt) override;
		};
	}
}