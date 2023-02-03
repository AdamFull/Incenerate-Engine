#pragma once

namespace engine
{
	namespace ecs
	{
		struct FPhysicsShapeComponent
		{
			int32_t type{ 0 };

			// For box/cylinder
			glm::vec3 sizes{ 1.f };
			// For capsule/cone/sphere
			float radius{ 1.f };
			float height{ 1.f };
		};

		void to_json(nlohmann::json& json, const FPhysicsShapeComponent& type);
		void from_json(const nlohmann::json& json, FPhysicsShapeComponent& type);
	}
}