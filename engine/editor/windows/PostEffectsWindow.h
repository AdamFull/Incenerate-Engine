#pragma once

#include "WindowBase.h"

namespace engine
{
	namespace editor
	{
		class CEditorPostEffects : public IEditorWindow
		{
		public:
			CEditorPostEffects(const std::string& sname) { name = sname; }
			virtual ~CEditorPostEffects() override = default;

			void create() override;
			void __draw(float fDt) override;
		};
	}
}