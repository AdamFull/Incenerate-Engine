#pragma once

#include "WindowBase.h"

namespace engine
{
	namespace ecs
	{
		class ISystem;
	}

	namespace editor
	{
		class CEditorPerfomanceView : public IEditorWindow
		{
		public:
			CEditorPerfomanceView(const std::string& sname) { name = sname; }
			virtual ~CEditorPerfomanceView() override = default;

			void create() override;
			void __draw(float fDt) override;
		private:
			void expand_create(ecs::ISystem* system);
			void expand_update(ecs::ISystem* system);
		};
	}
}