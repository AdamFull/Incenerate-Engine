#pragma once

#include "EngineStructures.h"
#include "PhysicsShapeComponent.h"

namespace engine
{
	namespace ecs
	{
		struct FRigidBodyComponent
		{
			float mass{ 1.f };
			size_t object_id{ invalid_index };
			FPhysicsShapeComponent shape;
		};

		void to_json(nlohmann::json& json, const FRigidBodyComponent& type);
		void from_json(const nlohmann::json& json, FRigidBodyComponent& type);
	}
}