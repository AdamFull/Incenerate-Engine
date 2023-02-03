#pragma once

#include "EngineStructures.h"
#include "PhysicsShapeComponent.h"

namespace engine
{
	namespace ecs
	{
		struct FColliderComponent
		{
			FPhysicsShapeComponent shape;
		};

		void to_json(nlohmann::json& json, const FColliderComponent& type);
		void from_json(const nlohmann::json& json, FColliderComponent& type);
	}
}