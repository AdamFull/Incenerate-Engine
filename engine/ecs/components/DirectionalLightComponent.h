#pragma once

#include <utility/uparse.hpp>

namespace engine
{
	namespace ecs
	{
		struct FDirectionalLightComponent
		{
			glm::vec3 color{ 1.f };
			glm::vec3 direction{ 0.f, 1.f, 0.f };
			float intencity{ 1.f };
			bool castShadows{ false };
		};

		struct FDirectionalLightCommit
		{
			alignas(16) glm::vec3 direction;
			alignas(16) glm::vec3 color;
			alignas(4) float intencity;
			alignas(4) bool castShadows;
		};

		void to_json(nlohmann::json& json, const FDirectionalLightComponent& type);
		void from_json(const nlohmann::json& json, FDirectionalLightComponent& type);
	}
}