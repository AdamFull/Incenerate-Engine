#pragma once

#include <utility/utime.hpp>

namespace engine
{
	namespace ecs
	{
		class ISystem
		{
		public:
			virtual ~ISystem() {}

			void create();
			void update(float fDt);
		protected:
			virtual void __create() = 0;
			virtual void __update(float fDf) = 0;

			utl::stopwatch sw;
			float ct{ 0.f }, ut{ 0.f };
		};
	}
}