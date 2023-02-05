#pragma once

namespace engine
{
	namespace ecs
	{
		struct FDirectionalLightComponent
		{
			glm::vec3 color{ 1.f };
			float intencity{ 1.f };
			bool castShadows{ 0 };

			float cascadeSplits[SHADOW_MAP_CASCADE_COUNT];
			float splitDepths[SHADOW_MAP_CASCADE_COUNT];
			glm::mat4 cascadeViewProj[SHADOW_MAP_CASCADE_COUNT];
		};

		struct FDirectionalLightCommit
		{
			//float cascadeSplits[SHADOW_MAP_CASCADE_COUNT];
			//glm::mat4 cascadeViewProj[SHADOW_MAP_CASCADE_COUNT];
			alignas(16) glm::vec3 direction;
			alignas(16) glm::vec3 color;
			alignas(4) float intencity;
			alignas(4) int castShadows;
		};

		void to_json(nlohmann::json& json, const FDirectionalLightComponent& type);
		void from_json(const nlohmann::json& json, FDirectionalLightComponent& type);
	}
}