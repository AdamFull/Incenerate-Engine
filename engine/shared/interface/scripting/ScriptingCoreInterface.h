#pragma once

#include "ScriptSourceInterface.h"

#include <memory>

namespace engine
{
	namespace scripting
	{
		class IScriptingCoreInterface
		{
		public:
			virtual ~IScriptingCoreInterface() = default;

			virtual void create() = 0;
			virtual void update() = 0;

			virtual size_t addSource(const std::string& name, const std::string& filepath) = 0;
			virtual size_t addSource(const std::string& name, std::unique_ptr<IScriptSourceInterface>&& source) = 0;
			virtual void removeSource(const std::string& name) = 0;
			virtual void removeSource(size_t id) = 0;
			virtual const std::unique_ptr<IScriptSourceInterface>& getSource(const std::string& name) = 0;
			virtual const std::unique_ptr<IScriptSourceInterface>& getSource(size_t id) = 0;
		};
	}
}