#include "ScriptSource.h"

#include "filesystem/vfs_helper.h"

using namespace engine::filesystem;
using namespace engine::scripting;

CScriptSource::CScriptSource(IVirtualFileSystemInterface* vfs_ptr) :
	m_pVFS(vfs_ptr)
{

}

void CScriptSource::load(const std::string& filepath, sol::state& lua)
{
	if (m_pVFS->exists(filepath))
	{
		m_pEnvironment = std::make_unique<sol::environment>(lua, sol::create, lua.globals());

		if (m_pEnvironment)
		{
			std::string scriptdata;
			m_pVFS->readFile(filepath, scriptdata);
			auto res = lua.script(scriptdata, *m_pEnvironment, sol::script_pass_on_error);

			if (!res.valid())
			{
				auto error = res.get<sol::error>();
				//log_error(error.what());
			}

			m_pOnCreate = std::make_unique<sol::protected_function>(m_pEnvironment->get<sol::protected_function>("onCreate"));
			if (!m_pOnCreate || !m_pOnCreate->valid())
				m_pOnCreate.reset();

			m_pOnUpdate = std::make_unique<sol::protected_function>(m_pEnvironment->get<sol::protected_function>("onUpdate"));
			if (!m_pOnUpdate || !m_pOnUpdate->valid())
				m_pOnUpdate.reset();

			m_pOnDestroy = std::make_unique<sol::protected_function>(m_pEnvironment->get<sol::protected_function>("onDestroy"));
			if (!m_pOnDestroy || !m_pOnDestroy->valid())
				m_pOnDestroy.reset();

			lua.collect_garbage();
		}
	}
}

void CScriptSource::callOnCreate()
{
	if (m_pOnCreate && m_pOnCreate->valid())
	{
		auto result = m_pOnCreate->call();
		if (!result.valid())
		{
			auto err = result.get<sol::error>();
			//log_error(err.what());
		}
	}
}

void CScriptSource::callOnUpdate(float fDT)
{
	if (m_pOnUpdate && m_pOnUpdate->valid())
	{
		auto result = m_pOnUpdate->call(fDT);
		if (!result.valid())
		{
			auto err = result.get<sol::error>();
			//log_error(err.what());
		}
	}
}

void CScriptSource::callOnDestroy()
{
	if (m_pOnDestroy && m_pOnDestroy->valid())
	{
		auto result = m_pOnDestroy->call();
		if (!result.valid())
		{
			auto err = result.get<sol::error>();
			//log_error(err.what());
		}
	}
}