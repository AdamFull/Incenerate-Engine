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

			virtual void create();
			void update(float fDt);

			const float createElapsed() const;
			const float updateElapsed() const;

			const std::string& getName() const;

			const std::vector<std::unique_ptr<ISystem>>& getSubSystems() const;
		protected:
			virtual void __create() = 0;
			virtual void __update(float fDf) = 0;

			entt::registry* registry{ nullptr };
			std::string name;
			utl::stopwatch sw;
			float ct{ 0.f }, ut{ 0.f };
			std::vector <std::unique_ptr<ISystem>> vSubSystems;
		};
	}
}