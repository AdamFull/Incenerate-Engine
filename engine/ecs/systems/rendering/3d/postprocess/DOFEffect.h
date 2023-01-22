#pragma once

#include "ecs/components/fwd.h"

namespace engine
{
	namespace ecs
	{
		class CDOFEffect
		{
		public:
			void create();
			void update();
			size_t render(FCameraComponent* camera, size_t depth_source, size_t in_source, size_t out_source);
		private:
			void init();

			size_t shader_dof{ invalid_index };
			size_t shader_blur{ invalid_index };
			size_t blur_image{ invalid_index };
			size_t temp_image{ invalid_index };
		};
	}
}