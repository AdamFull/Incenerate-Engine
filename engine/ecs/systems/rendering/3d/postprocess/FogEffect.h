#pragma once

#include "ecs/components/fwd.h"

namespace engine
{
	namespace ecs
	{
		class CFogEffect
		{
		public:
			void create();
			size_t render(FCameraComponent* camera, size_t depth_source, size_t in_source, size_t out_source);
		private:
			graphics::CAPIHandle* graphics{ nullptr };
			size_t shader_fog{ invalid_index };
		};
	}
}