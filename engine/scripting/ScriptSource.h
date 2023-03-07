#pragma once

#include <lua/sol.hpp>
#include <utility/logger/logger.h>

namespace engine
{
	namespace scripting
	{
		class CScriptSource
		{
		public:
			friend class CScriptingCore;

			void load(const std::filesystem::path& filepath, sol::state& lua);

			void callOnCreate();
			void callOnUpdate(float fDT);
			void callOnDestroy();
		private:
			std::unique_ptr<sol::environment> pEnv;
			std::unique_ptr<sol::protected_function> pOnCreate;
			std::unique_ptr<sol::protected_function> pOnUpdate;
			std::unique_ptr<sol::protected_function> pOnDestroy;
		};
	}
}