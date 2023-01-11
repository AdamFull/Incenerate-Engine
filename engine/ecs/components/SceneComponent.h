#pragma once

#include <utility/uparse.hpp>

namespace engine
{
	namespace ecs
	{
		struct FSceneComponent
		{
			std::string source;
		};

		void to_json(nlohmann::json& json, const FSceneComponent& type);
		void from_json(const nlohmann::json& json, FSceneComponent& type);
	}
}