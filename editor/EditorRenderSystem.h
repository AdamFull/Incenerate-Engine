#pragma once

#include "ecs/systems/rendering/BaseGraphicsSystem.h"

namespace engine
{
	namespace editor
	{
		class CEditorRenderSystem : public ecs::CBaseGraphicsSystem
		{
		public:
			CEditorRenderSystem() { name = "Editor present system"; }
			virtual ~CEditorRenderSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		};
	}
}