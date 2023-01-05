#pragma once

#include "graphics/rendering/RenderStage.h"

#include "ecs/core/System.hpp"
#include "ecs/core/Event.hpp"

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
			void onReCreate(CEvent& event);
		private:
			graphics::FCIStage stageCI;
			size_t stageId{ invalid_index }, image{ invalid_index }, shader{ invalid_index };
		};
	}
}