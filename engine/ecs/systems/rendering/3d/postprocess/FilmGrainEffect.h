#pragma once

#include "ecs/components/fwd.h"

namespace engine
{
	namespace ecs
	{
		class CFilmGrainEffect
		{
		public:
			void create();
			size_t render(FCameraComponent* camera, float time, size_t in_source, size_t out_source);
		private:
			graphics::CAPIHandle* graphics{ nullptr };

			size_t shader_grain{ invalid_index };
		};
	}
}