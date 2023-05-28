#pragma once

namespace engine
{
	namespace ecs
	{
		struct FPointLightComponent
		{
			glm::vec3 color{ 1.f };
			float intencity{ 1.f };
			float radius{ 1.f };
			int shadowIndex{ -1 };
			bool castShadows{ false };
		};

		struct FOmniShadowCommit
		{
			alignas(4) float farPlane;
			alignas(4) int index;
		};

		struct FPointLightCommit
		{
			alignas(16) glm::vec3 position;
			alignas(16) glm::vec3 color;
			alignas(4) float intencity;
			alignas(4) float radius;
			alignas(4) int shadowIndex;
			alignas(4) int castShadows;
		};

		void to_json(nlohmann::json& json, const FPointLightComponent& type);
		void from_json(const nlohmann::json& json, FPointLightComponent& type);
	}
}