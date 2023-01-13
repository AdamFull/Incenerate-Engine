#pragma once

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
			void update_path(const std::filesystem::path& npath);
		private:
			std::filesystem::path currentPath{};
			std::vector<std::string> vCurrentDirData;

			std::filesystem::path workdir{};
			std::filesystem::path relative{};
		};
	}
}