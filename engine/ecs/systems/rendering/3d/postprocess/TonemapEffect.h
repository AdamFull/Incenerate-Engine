#pragma once

#include "ecs/components/fwd.h"

namespace engine
{
	namespace ecs
	{
		class CTonemapEffect
		{
		public:
			void create();
			size_t render(FCameraComponent* camera, size_t in_source, size_t out_source);
		private:
			size_t shader_tonemap{ invalid_index };
			graphics::CAPIHandle* graphics{ nullptr };
		};
	}
}