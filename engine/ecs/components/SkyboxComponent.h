#pragma once

#include <utility/uparse.hpp>

namespace engine
{
	namespace ecs
	{
		struct FSkyboxComponent
		{
			std::string source;
			size_t origin;
			size_t prefiltred;
			size_t irradiance;
		};

		void to_json(nlohmann::json& json, const FSkyboxComponent& type);
		void from_json(const nlohmann::json& json, FSkyboxComponent& type);
	}
}