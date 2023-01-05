#pragma once

#include "ecs/core/System.hpp"

namespace engine
{
	namespace ecs
	{
		class CScriptingSystem : public ISystem
		{
		public:
			virtual ~CScriptingSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}