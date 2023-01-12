#pragma once

#include <utility/uparse.hpp>

namespace engine
{
	namespace ecs
	{
		struct FSceneComponent
		{
			~FSceneComponent();
			void create(entt::entity node);

			std::string source;
			bool loaded{ false };
			entt::entity self;
		};

		void to_json(nlohmann::json& json, const FSceneComponent& type);
		void from_json(const nlohmann::json& json, FSceneComponent& type);
	}
}