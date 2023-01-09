#pragma once

#include "ecs/systems/BaseSystem.h"

namespace engine
{
	namespace ecs
	{
		class CDeferredPassSystem : public ISystem
		{
		public:
			virtual ~CDeferredPassSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		private:
			std::vector<std::unique_ptr<ISystem>> vSubSystems;
		};
	}
}