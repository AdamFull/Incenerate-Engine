#pragma once

namespace engine
{
	namespace ecs
	{
		struct FSpriteComponent
		{
			std::string source{ "" };
			size_t image{ invalid_index };
		};

		void to_json(nlohmann::json& json, const FSpriteComponent& type);
		void from_json(const nlohmann::json& json, FSpriteComponent& type);
	}
}