#pragma once

#include "ecs/systems/BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CEditorRenderSystem : public ISystem
		{
		public:
			virtual ~CEditorRenderSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}