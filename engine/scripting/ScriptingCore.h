#pragma once

#include <lua/sol.hpp>

namespace engine
{
	namespace scripting
	{
		class CScriptingCore
		{
		public:
			void create();
		private:
			sol::state lua;
		};
	}
}