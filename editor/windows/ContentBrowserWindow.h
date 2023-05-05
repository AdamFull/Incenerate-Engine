#pragma once

#include "event/interface/EventInterface.h"
#include "WindowBase.h"

namespace engine
{
	namespace editor
	{
		class CEditorContentBrowser : public IEditorWindow
		{
		public:
			CEditorContentBrowser(const std::string& sname) { name = sname; }
			virtual ~CEditorContentBrowser() override = default;

			void create() override;
			void __draw(float fDt) override;
		private:
			void OnProjectChanged(const std::unique_ptr<IEvent>& event);
			void update_path(const std::string& npath);
		private:
			std::string currentPath{};
			std::vector<std::string> vCurrentDirData;
			std::string selected;

			std::string workdir{};

			const char* open_popup_name{ nullptr };
		};
	}
}