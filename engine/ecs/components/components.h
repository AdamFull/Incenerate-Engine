#pragma once

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
			constexpr inline size_t audio = utl::type_hash<FAudioComponent>();
			constexpr inline size_t camera = utl::type_hash<FCameraComponent>();
			constexpr inline size_t directional_light = utl::type_hash<FDirectionalLightComponent>();
			constexpr inline size_t environment = utl::type_hash<FEnvironmentComponent>();
			constexpr inline size_t hierarchy = utl::type_hash<FHierarchyComponent>();
			constexpr inline size_t mesh = utl::type_hash<FMeshComponent>();
			constexpr inline size_t point_light = utl::type_hash<FPointLightComponent>();
			constexpr inline size_t scene = utl::type_hash<FSceneComponent>();
			constexpr inline size_t script = utl::type_hash<FScriptComponent>();
			constexpr inline size_t spot_light = utl::type_hash<FSpotLightComponent>();
			constexpr inline size_t sprite = utl::type_hash<FSpriteComponent>();
			constexpr inline size_t transform = utl::type_hash<FTransformComponent>();
		}
	}
}