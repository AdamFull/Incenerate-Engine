#pragma once

#include "WindowBase.h"

namespace engine
{
	namespace editor
	{
		class CEditorInspector : public IEditorWindow
		{
		public:
			CEditorInspector(const std::string& sname) { name = sname; }
			virtual ~CEditorInspector() override = default;

			void create() override;
			void __draw() override;
		};
	}
}