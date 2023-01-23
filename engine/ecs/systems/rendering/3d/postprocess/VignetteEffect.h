#pragma once

#include "ecs/components/fwd.h"

namespace engine
{
	namespace ecs
	{
		class CVignetteEffect
		{
		public:
			void create();
			size_t render(FCameraComponent* camera, size_t in_source, size_t out_source);
		private:
			size_t shader_vignette{ invalid_index };
		};
	}
}