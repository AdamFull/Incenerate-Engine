#pragma once

#include "BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CScriptingSystem : public ISystem
		{
		public:
			CScriptingSystem() { name = "Scripting system"; }
			virtual ~CScriptingSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}