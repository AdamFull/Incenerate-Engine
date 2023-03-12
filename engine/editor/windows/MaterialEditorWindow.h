#pragma once

#include "WindowBase.h"
#include "materialeditor/NodeAttribContainer.h"
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
			void makePopupMenuContent(const std::vector<FMaterialEditorGroupCreateInfo>& groups);
		private:
			size_t selected_material{ invalid_index };
			int32_t hovered_id{ 0 };

			std::unique_ptr<CNodeAttribContainer> pContainer;
			std::vector<FMaterialEditorGroupCreateInfo> vGroups;
		};
	}
}