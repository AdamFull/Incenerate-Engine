#pragma once

#include "ecs/systems/BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class C3DRenderSystem : public ISystem
		{
		public:
			C3DRenderSystem();
			virtual ~C3DRenderSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		private:
			std::vector <std::unique_ptr<ISystem>> vSubSystems;
		};
	}
}