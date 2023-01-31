#pragma once

namespace engine
{
	namespace ecs
	{
		struct FScriptComponent
		{
			std::string source;
			size_t data{ invalid_index };
			bool loaded{ false };
		};

		void to_json(nlohmann::json& json, const FScriptComponent& type);
		void from_json(const nlohmann::json& json, FScriptComponent& type);
	}
}