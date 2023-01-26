#include "ScriptingCore.h"

#include "lua_bindings.h"

using namespace engine::scripting;

void CScriptingCore::create()
{
    pScriptManager = std::make_unique<CObjectManager<CScriptSource>>();

    bind_glm_lib(lua);
    bind_components(lua);
}

void CScriptingCore::update()
{

}

size_t CScriptingCore::addSource(const std::string& name, const std::filesystem::path& filepath)
{
    auto script = std::make_unique<CScriptSource>();
    script->load(filepath, lua);
    return pScriptManager->add(name, std::move(script));
}

size_t CScriptingCore::addSource(const std::string& name, std::unique_ptr<CScriptSource>&& source)
{
    return pScriptManager->add(name, std::move(source));
}

void CScriptingCore::removeSource(const std::string& name)
{
    pScriptManager->remove(name);
}

void CScriptingCore::removeSource(size_t id)
{
    pScriptManager->remove(id);
}

const std::unique_ptr<CScriptSource>& CScriptingCore::getSource(const std::string& name)
{
    return pScriptManager->get(name);
}

const std::unique_ptr<CScriptSource>& CScriptingCore::getSource(size_t id)
{
    return pScriptManager->get(id);
}