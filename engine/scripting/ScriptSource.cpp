#include "ScriptSource.h"

#include "system/filesystem/filesystem.h"

using namespace engine::system;
using namespace engine::scripting;

void CScriptSource::load(const std::filesystem::path& filepath, sol::state& lua)
{
	auto fullpath = fs::get_workdir() / filepath;

	if (fs::is_file_exists(fullpath))
	{
		pEnv = std::make_unique<sol::environment>(lua, sol::create, lua.globals());

		if (pEnv)
		{
			auto path = fullpath.string();
			auto res = lua.script_file(path, *pEnv, sol::script_pass_on_error);

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