#pragma once

#include "DirectionalShadowSystem.h"
#include "OmniShadowSystem.h"

namespace engine
{
	namespace ecs
	{
		class CShadowPassSystem : public ISystem
		{
		public:
			virtual ~CShadowPassSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		private:
			std::vector<std::unique_ptr<ISystem>> vSubSystems;
		};
	}
}