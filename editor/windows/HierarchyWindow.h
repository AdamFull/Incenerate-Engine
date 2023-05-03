#pragma once

#include "WindowBase.h"

namespace engine
{
	namespace editor
	{
		class CEditorHierarchy : public IEditorWindow
		{
		public:
			CEditorHierarchy(const std::string& sname) { name = sname; }
			virtual ~CEditorHierarchy() override = default;

			void create() override;
			void __draw(float fDt) override;
		private:
			entt::entity selected_entity{ entt::null };
			entt::entity copy_entity{ entt::null };
			void buildHierarchy(const entt::entity& entity);
		};
	}
}