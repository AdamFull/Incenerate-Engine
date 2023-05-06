#pragma once

#include <string>

namespace engine
{
	namespace scripting
	{
		class IScriptSourceInterface
		{
		public:
			virtual ~IScriptSourceInterface() = default;

			virtual void callOnCreate() = 0;
			virtual void callOnUpdate(float fDT) = 0;
			virtual void callOnDestroy() = 0;
		};
	}
}