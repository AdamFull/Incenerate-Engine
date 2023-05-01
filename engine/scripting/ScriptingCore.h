#pragma once

#include "ScriptSource.h"
#include "shared/ObjectManager.hpp"

namespace engine
{
	namespace scripting
	{
		class CScriptingCore
		{
		public:
			void create();
			void update();

			size_t addSource(const std::string& name, const std::string& filepath);
			size_t addSource(const std::string& name, std::unique_ptr<CScriptSource>&& source);
			void removeSource(const std::string& name);
			void removeSource(size_t id);
			const std::unique_ptr<CScriptSource>& getSource(const std::string& name);
			const std::unique_ptr<CScriptSource>& getSource(size_t id);
		private:
			sol::state lua;

			std::unique_ptr<CObjectManager<CScriptSource>> pScriptManager;
		};
	}
}