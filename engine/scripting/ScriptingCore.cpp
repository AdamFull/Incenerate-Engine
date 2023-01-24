#include "ScriptingCore.h"

#include "lua_bindings.h"

using namespace engine::scripting;

struct vars {
    int boop = 0;
};

void CScriptingCore::create()
{
    bind_glm_lib(lua);

    lua.script(
        "first = vec4.new(2.0)\n"
        "second = vec3.new(2.5)\n"
        "result = first * vec4.new(second, 0.5)");

    auto result = lua.get<glm::vec4>("result");
}