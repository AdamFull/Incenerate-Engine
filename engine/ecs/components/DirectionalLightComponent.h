#pragma once

#include <utility/uparse.hpp>

namespace engine
{
	namespace ecs
	{
		struct FDirectionalLightComponent
		{
			glm::vec3 color;
			float intencity;
			bool castShadows{ true };
		};

		struct FDirectionalLightCommit
		{
			alignas(16) glm::vec3 direction;
			alignas(16) glm::vec3 color;
			alignas(4) float intencity;
		};

		void to_json(nlohmann::json& json, const FDirectionalLightComponent& type);
		void from_json(const nlohmann::json& json, FDirectionalLightComponent& type);
	}
}