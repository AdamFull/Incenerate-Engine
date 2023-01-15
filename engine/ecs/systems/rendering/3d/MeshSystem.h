#pragma once

#include "ecs/systems/BaseSystem.h"
#include "ecs/components/fwd.h"

namespace engine
{
	namespace ecs
	{
		class CMeshSystem : public ISystem
		{
		public:
			CMeshSystem() { name = "Rendering mesh system"; }
			virtual ~CMeshSystem() override = default;

			void __create() override;
			void __update(float fDt) override;

		private:
			void draw(const FCameraComponent* camera, graphics::EAlphaMode alphaMode);
		};
	}
}