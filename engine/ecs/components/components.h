#pragma once

#include <utility/upattern.hpp>

#include "AudioComponent.h"
#include "CameraComponent.h"
#include "DirectionalLightComponent.h"
#include "EnvironmentComponent.h"
#include "HierarchyComponent.h"
#include "MeshComponent.h"
#include "PointLightComponent.h"
#include "SceneComponent.h"
#include "ScriptComponent.h"
#include "SpotLightComponent.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"

namespace engine
{
	namespace ecs
	{
		namespace component
		{
			constexpr size_t audio = utl::type_hash<FAudioComponent>();
			constexpr size_t camera = utl::type_hash<FCameraComponent>();
			constexpr size_t directional_light = utl::type_hash<FDirectionalLightComponent>();
			constexpr size_t environment = utl::type_hash<FEnvironmentComponent>();
			constexpr size_t hierarchy = utl::type_hash<FHierarchyComponent>();
			constexpr size_t mesh = utl::type_hash<FMeshComponent>();
			constexpr size_t point_light = utl::type_hash<FPointLightComponent>();
			constexpr size_t scene = utl::type_hash<FSceneComponent>();
			constexpr size_t script = utl::type_hash<FScriptComponent>();
			constexpr size_t spot_light = utl::type_hash<FSpotLightComponent>();
			constexpr size_t sprite = utl::type_hash<FSpriteComponent>();
			constexpr size_t transform = utl::type_hash<FTransformComponent>();
		}
	}
}