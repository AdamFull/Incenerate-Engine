#include "ScriptingCore.h"

#include "lua_bindings.h"

using namespace engine::scripting;

CScriptingCore::CScriptingCore(filesystem::IVirtualFileSystemInterface* vfs_ptr) :
    m_pVFS(vfs_ptr)
{

}

void CScriptingCore::create()
{
    pScriptManager = std::make_unique<CObjectManager<IScriptSourceInterface>>();

    bind_glm_lib(*m_pLuaMachine);
    //bind_components(lua);
}

void CScriptingCore::update()
{

}

size_t CScriptingCore::addSource(const std::string& name, const std::string& filepath)
{
    auto script = std::make_unique<CScriptSource>(m_pVFS);
    script->load(filepath, *m_pLuaMachine);
    return pScriptManager->add(name, std::move(script));
}

size_t CScriptingCore::addSource(const std::string& name, std::unique_ptr<IScriptSourceInterface>&& source)
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

const std::unique_ptr<IScriptSourceInterface>& CScriptingCore::getSource(const std::string& name)
{
    return pScriptManager->get(name);
}

const std::unique_ptr<IScriptSourceInterface>& CScriptingCore::getSource(size_t id)
{
    return pScriptManager->get(id);
}