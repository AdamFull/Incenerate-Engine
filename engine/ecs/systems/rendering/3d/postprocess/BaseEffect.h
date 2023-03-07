#pragma once

#include "ecs/components/fwd.h"

namespace engine
{
	namespace ecs
	{
		class CBaseEffect
		{
		public:
			virtual void create();
			virtual void update(FCameraComponent*);
		private:
			graphics::CAPIHandle* graphics{ nullptr };

			std::unordered_map<std::string, size_t> images;
		};
	}
}