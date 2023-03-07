#pragma once

namespace engine
{
	namespace ecs
	{
		struct FDirectionalLightComponent
		{
			glm::vec3 color{ 1.f };
			float intencity{ 1.f };
			bool castShadows{ false };

			std::array<float, SHADOW_MAP_CASCADE_COUNT> cascadeSplits;
			std::array<float, SHADOW_MAP_CASCADE_COUNT> splitDepths;
			std::array<glm::mat4, SHADOW_MAP_CASCADE_COUNT> cascadeViewProj;
		};

		struct FDirectionalLightCommit
		{
			std::array<float, SHADOW_MAP_CASCADE_COUNT> cascadeSplits;
			std::array<glm::mat4, SHADOW_MAP_CASCADE_COUNT> cascadeViewProj;
			alignas(16) glm::vec3 direction;
			alignas(16) glm::vec3 color;
			alignas(4) float intencity;
			alignas(4) int castShadows;
		};

		void to_json(nlohmann::json& json, const FDirectionalLightComponent& type);
		void from_json(const nlohmann::json& json, FDirectionalLightComponent& type);
	}
}