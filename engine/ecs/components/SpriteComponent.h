#pragma once

#include <utility/uparse.hpp>

namespace engine
{
	namespace ecs
	{
		struct FSpriteComponent
		{
			std::string source;
			size_t image;
		};

		void to_json(nlohmann::json& json, const FSpriteComponent& type);
		void from_json(const nlohmann::json& json, FSpriteComponent& type);
	}
}