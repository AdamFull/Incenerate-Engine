#pragma once

#include <lua/sol.hpp>
#include <logger/logger.h>
#include <interface/filesystem/VirtualFileSystemInterface.h>

namespace engine
{
	namespace scripting
	{
		class CScriptSource
		{
		public:
			friend class CScriptingCore;

			CScriptSource() = default;
			CScriptSource(filesystem::IVirtualFileSystemInterface* vfs_ptr);

			void load(const std::string& filepath, sol::state& lua);

			void callOnCreate();
			void callOnUpdate(float fDT);
			void callOnDestroy();
		private:
			std::unique_ptr<sol::environment> m_pEnvironment;
			std::unique_ptr<sol::protected_function> m_pOnCreate;
			std::unique_ptr<sol::protected_function> m_pOnUpdate;
			std::unique_ptr<sol::protected_function> m_pOnDestroy;
			filesystem::IVirtualFileSystemInterface* m_pVFS{ nullptr };
		};
	}
}