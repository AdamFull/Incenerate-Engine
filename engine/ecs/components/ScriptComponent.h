#pragma once

#include <utility/uparse.hpp>

namespace engine
{
	namespace ecs
	{
		struct FScriptComponent
		{
			std::string source;
		};

		void to_json(nlohmann::json& json, const FScriptComponent& type);
		void from_json(const nlohmann::json& json, FScriptComponent& type);
	}
}