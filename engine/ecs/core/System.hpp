#pragma once

#include "Type.hpp"
#include <set>

namespace engine
{
	namespace ecs
	{
		class ISystem
		{
		public:
			virtual void create() = 0;
			virtual void update(float fDt) = 0;
		protected:
			std::set<Entity> mEntities;
		};
	}
}