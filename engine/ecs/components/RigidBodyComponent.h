#pragma once

#include "EngineStructures.h"

namespace engine
{
	namespace ecs
	{
		struct FRigidBodyComponent
		{
			float mass{ 1.f };
			size_t object_id{ invalid_index };

			EPhysicsShapeType type{ EPhysicsShapeType::eBox };

			// For box/cylinder
			glm::vec3 sizes{ 1.f };
			// For capsule/cone/sphere
			float radius{ 1.f };
			float height{ 1.f };
		};

		void to_json(nlohmann::json& json, const FRigidBodyComponent& type);
		void from_json(const nlohmann::json& json, FRigidBodyComponent& type);
	}
}