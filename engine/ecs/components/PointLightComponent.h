#pragma once

#include <utility/uparse.hpp>

namespace engine
{
	namespace ecs
	{
		struct FPointLightComponent
		{
			glm::vec3 color;
			float intencity{ 1.f };
			float radius{ 1.f };
			bool castShadows{ false };
		};

		struct FPointLightCommit
		{
			alignas(16) glm::vec3 position;
			alignas(16) glm::vec3 color;
			alignas(4) float intencity;
			alignas(4) float radius;
			alignas(4) bool castShadows;
		};

		void to_json(nlohmann::json& json, const FPointLightComponent& type);
		void from_json(const nlohmann::json& json, FPointLightComponent& type);
	}
}