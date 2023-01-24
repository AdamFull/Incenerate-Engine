#include "lua_bindings.h"

#include "ecs/components/components.h"

using namespace engine::ecs;

namespace engine
{
	namespace scripting
	{
		void bind_components(sol::state& lua)
		{
			lua.new_usertype<FTransformComponent>(
				"FTransformComponent",
				sol::constructors<FTransformComponent()>(),
				"position", &FTransformComponent::position,
				"rotation", &FTransformComponent::rotation,
				"scale", &FTransformComponent::scale,
				"rel_position", &FTransformComponent::rposition,
				"rel_rotation", &FTransformComponent::rrotation,
				"rel_scale", &FTransformComponent::rscale,
				"model", &FTransformComponent::model,
				"model_old", &FTransformComponent::model_old
				);

			lua.new_usertype<FHierarchyComponent>(
				"FHierarchyComponent",
				sol::constructors<FHierarchyComponent()>(),
				"name", &FHierarchyComponent::name,
				"parent", &FHierarchyComponent::parent,
				"children", &FHierarchyComponent::children
				);

			lua.new_usertype<FAudioComponent>(
				"FAudioComponent",
				sol::constructors<FAudioComponent()>()
				);

			lua.new_usertype<FCameraComponent>(
				"FCameraComponent",
				sol::constructors<FCameraComponent()>()
				);

			lua.new_usertype<FEnvironmentComponent>(
				"FEnvironmentComponent",
				sol::constructors<FEnvironmentComponent()>()
				);

			lua.new_usertype<FMeshComponent>(
				"FMeshComponent",
				sol::constructors<FMeshComponent()>()
				);

			lua.new_usertype<FSceneComponent>(
				"FSceneComponent",
				sol::constructors<FSceneComponent()>()
				);

			lua.new_usertype<FScriptComponent>(
				"FScriptComponent",
				sol::constructors<FScriptComponent()>()
				);

			lua.new_usertype<FSpriteComponent>(
				"FSpriteComponent",
				sol::constructors<FSpriteComponent()>()
				);

			lua.new_usertype<FDirectionalLightComponent>(
				"FDirectionalLightComponent",
				sol::constructors<FDirectionalLightComponent()>()
				);
			
			lua.new_usertype<FPointLightComponent>(
				"FPointLightComponent",
				sol::constructors<FPointLightComponent()>()
				);
			
			lua.new_usertype<FSpotLightComponent>(
				"FSpotLightComponent",
				sol::constructors<FSpotLightComponent()>()
				);
		}
	}
}