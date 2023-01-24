#pragma once

#include <lua/sol.hpp>

namespace engine
{
	namespace scripting
	{
		void bind_glm_lib(sol::state& lua);
		void bind_components(sol::state& lua);
	}
}