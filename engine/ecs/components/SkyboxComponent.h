#pragma once

#include <utility/uparse.hpp>

namespace engine
{
	namespace ecs
	{
		struct FSkyboxComponent
		{
			std::string source;
			size_t vbo_id{ invalid_index };
			size_t shader_id{ invalid_index };
			size_t origin{ invalid_index };
			size_t brdflut{ invalid_index };
			size_t prefiltred{ invalid_index };
			size_t irradiance{ invalid_index };
			bool loaded{ false };
		};

		void to_json(nlohmann::json& json, const FSkyboxComponent& type);
		void from_json(const nlohmann::json& json, FSkyboxComponent& type);
	}
}