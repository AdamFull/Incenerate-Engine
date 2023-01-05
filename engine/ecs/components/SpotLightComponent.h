#pragma once

#include <utility/uparse.hpp>

namespace engine
{
	namespace ecs
	{
		struct FSpotLightComponent
		{
			glm::vec3 color;
			float intencity;
			float innerAngle;
			float outerAngle;
			bool toTarget{ true };
			bool castShadows{ true };
		};

		struct FSpotLightCommit
		{
			glm::mat4 shadowView;
			alignas(16) glm::vec3 position;
			alignas(16) glm::vec3 direction;
			alignas(16) glm::vec3 color;
			alignas(4) float intencity;
			alignas(4) float innerConeAngle;
			alignas(4) float outerConeAngle;
			alignas(4) bool toTarget{ true };
		};

		void to_json(nlohmann::json& json, const FSpotLightComponent& type);
		void from_json(const nlohmann::json& json, FSpotLightComponent& type);
	}
}