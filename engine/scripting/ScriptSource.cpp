#include "ScriptSource.h"

#include "Engine.h"

#include "filesystem/vfs_helper.h"

using namespace engine::filesystem;
using namespace engine::scripting;

void CScriptSource::load(const std::string& filepath, sol::state& lua)
{
	if (EGFilesystem->exists(filepath))
	{
		pEnv = std::make_unique<sol::environment>(lua, sol::create, lua.globals());

		if (pEnv)
		{
			std::string scriptdata;
			EGFilesystem->readFile(filepath, scriptdata);
			auto res = lua.script(scriptdata, *pEnv, sol::script_pass_on_error);

			if (!res.valid())
			{
				auto error = res.get<sol::error>();
				log_error(error.what());
			}

			pOnCreate = std::make_unique<sol::protected_function>(pEnv->get<sol::protected_function>("onCreate"));
			if (!pOnCreate || !pOnCreate->valid())
				pOnCreate.reset();

			pOnUpdate = std::make_unique<sol::protected_function>(pEnv->get<sol::protected_function>("onUpdate"));
			if (!pOnUpdate || !pOnUpdate->valid())
				pOnUpdate.reset();

			pOnDestroy = std::make_unique<sol::protected_function>(pEnv->get<sol::protected_function>("onDestroy"));
			if (!pOnDestroy || !pOnDestroy->valid())
				pOnDestroy.reset();

			lua.collect_garbage();
		}
	}
}

void CScriptSource::callOnCreate()
{
	if (pOnCreate && pOnCreate->valid())
	{
		auto result = pOnCreate->call();
		if (!result.valid())
		{
			auto err = result.get<sol::error>();
			log_error(err.what());
		}
	}
}

void CScriptSource::callOnUpdate(float fDT)
{
	if (pOnUpdate && pOnUpdate->valid())
	{
		auto result = pOnUpdate->call(fDT);
		if (!result.valid())
		{
			auto err = result.get<sol::error>();
			log_error(err.what());
		}
	}
}

void CScriptSource::callOnDestroy()
{
	if (pOnDestroy && pOnDestroy->valid())
	{
		auto result = pOnDestroy->call();
		if (!result.valid())
		{
			auto err = result.get<sol::error>();
			log_error(err.what());
		}
	}
}