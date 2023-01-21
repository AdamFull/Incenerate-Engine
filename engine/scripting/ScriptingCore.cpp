#include "ScriptingCore.h"

using namespace engine::scripting;

struct vars {
    int boop = 0;
};

void CScriptingCore::create()
{
    lua.new_usertype<vars>("vars", "boop", &vars::boop);
    lua.script("beep = vars.new()\n"
        "beep.boop = 1");
    assert(lua.get<vars>("beep").boop == 1);
}