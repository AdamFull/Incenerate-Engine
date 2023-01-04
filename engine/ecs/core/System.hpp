#pragma once

#include "Type.hpp"
#include <set>

#include <utility/utime.hpp>

namespace engine
{
	namespace ecs
	{
		class ISystem
		{
		public:
			friend class CSystemManager;

			virtual ~ISystem() {}

			void create();
			void update(float fDt);
		protected:
			virtual void __create() = 0;
			virtual void __update(float fDf) = 0;

			float ct{ 0.f }, ut{ 0.f };
			utl::stopwatch sw;
			std::set<Entity> mEntities;
		};
	}
}