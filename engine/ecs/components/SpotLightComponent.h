#pragma once

#include <utility/uparse.hpp>

namespace engine
{
	namespace ecs
	{
		struct FSpotLightComponent
		{
			glm::vec3 color{ 1.f };
			glm::vec3 target{ 0.f, 0.f, 1.f };
			float intencity{ 1.f };
			float innerAngle{ 0.f };
			float outerAngle{ 0.78f };
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
			alignas(4) float lightAngleScale;
			alignas(4) float lightAngleOffset;
			alignas(4) int toTarget{ 0 };
			alignas(4) int castShadows{ 0 };
		};

		void to_json(nlohmann::json& json, const FSpotLightComponent& type);
		void from_json(const nlohmann::json& json, FSpotLightComponent& type);
	}
}