#pragma once

#include "WindowBase.h"

namespace engine
{
	namespace editor
	{
		class CRenderDebugWindow : public IEditorWindow
		{
		public:
			CRenderDebugWindow(const std::string& sname) { name = sname; }
			virtual ~CRenderDebugWindow() override = default;

			void create() override;
			void __draw(float fDt) override;
		};
	}
}