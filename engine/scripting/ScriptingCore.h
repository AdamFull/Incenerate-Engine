#pragma once

#include "ScriptSource.h"
#include <ObjectManager.hpp>
#include <interface/scripting/ScriptingCoreInterface.h>

namespace engine
{
	namespace scripting
	{
		class CScriptingCore final : public IScriptingCoreInterface
		{
		public:
			CScriptingCore() = default;
			CScriptingCore(filesystem::IVirtualFileSystemInterface* vfs_ptr);

			void create() override;
			void update() override;

			size_t addSource(const std::string& name, const std::string& filepath) override;
			size_t addSource(const std::string& name, std::unique_ptr<IScriptSourceInterface>&& source) override;
			void removeSource(const std::string& name) override;
			void removeSource(size_t id) override;
			const std::unique_ptr<IScriptSourceInterface>& getSource(const std::string& name) override;
			const std::unique_ptr<IScriptSourceInterface>& getSource(size_t id) override;
		private:
			std::unique_ptr<sol::state> m_pLuaMachine;
			std::unique_ptr<CObjectManager<IScriptSourceInterface>> pScriptManager;
			filesystem::IVirtualFileSystemInterface* m_pVFS{ nullptr };
		};
	}
}