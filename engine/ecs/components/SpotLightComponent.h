#pragma once

#include <utility/uparse.hpp>

namespace engine
{
	namespace ecs
	{
		struct FSpotLightComponent
		{
			glm::vec3 color;
			float intencity{ 1.f };
			float innerAngle{ 0.92f };
			float outerAngle{ 0.98f };
			bool toTarget{ false };
			bool castShadows{ false };
		};

		struct FSpotLightCommit
		{
			glm::mat4 shadowView;
			alignas(16) glm::vec3 position;
			alignas(16) glm::vec3 direction;
			alignas(16) glm::vec3 color;
			alignas(4) float intencity;
			alignas(4) float innerAngle;
			alignas(4) float outerAngle;
			alignas(4) bool toTarget{ false };
			alignas(4) bool castShadows;
		};

		void to_json(nlohmann::json& json, const FSpotLightComponent& type);
		void from_json(const nlohmann::json& json, FSpotLightComponent& type);
	}
}