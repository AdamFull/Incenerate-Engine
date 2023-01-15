#pragma once

#include "WindowBase.h"

namespace engine
{
	namespace editor
	{
		class CEditorPerfomanceView : public IEditorWindow
		{
		public:
			CEditorPerfomanceView(const std::string& sname) { name = sname; }
			virtual ~CEditorPerfomanceView() override = default;

			void create() override;
			void __draw(float fDt) override;
		};
	}
}