#include "lua_bindings.h"

#include "ecs/components/components.h"

using namespace engine::ecs;

namespace engine
{
	namespace scripting
	{
		void bind_components(sol::state& lua)
		{
			auto transform = lua.new_usertype<FTransformComponent>("FTransformComponent", sol::constructors<FTransformComponent()>());
			transform.set("position", &FTransformComponent::position);
			transform.set("rotation", &FTransformComponent::rotation);
			transform.set("scale", &FTransformComponent::scale);
			transform.set("rel_position", &FTransformComponent::rposition);
			transform.set("rel_rotation", &FTransformComponent::rrotation);
			transform.set("rel_scale", &FTransformComponent::rscale);
			transform.set("model", &FTransformComponent::model);
			transform.set("model_old", &FTransformComponent::model_old);

			auto hierarchy = lua.new_usertype<FHierarchyComponent>("FHierarchyComponent", sol::constructors<FHierarchyComponent()>());
			hierarchy.set("name", &FHierarchyComponent::name);
			hierarchy.set("parent", &FHierarchyComponent::parent);
			hierarchy.set("children", &FHierarchyComponent::children);

			auto audio = lua.new_usertype<FAudioComponent>("FAudioComponent", sol::constructors<FAudioComponent()>());
			audio.set("source", &FAudioComponent::source);
			audio.set("descriptor", &FAudioComponent::asource);
			audio.set("gain", &FAudioComponent::gain);
			audio.set("pitch", &FAudioComponent::pitch);
			audio.set("velocity", &FAudioComponent::velocity);
			audio.set("loop", &FAudioComponent::loop);
			audio.set("playing", &FAudioComponent::playing);
			

			lua.new_enum <ECameraType>(
					"ECameraType",
				{
					{"orthographic", ECameraType::eOrthographic},
					{"perspective", ECameraType::ePerspective}
				}
					);

			auto frustum = lua.new_usertype<FFrustum>("FFrustum", sol::constructors<FFrustum()>());
			frustum.set_function("update", &FFrustum::update);
			frustum.set_function("checkPoint", &FFrustum::checkPoint);
			frustum.set_function("checkSphere", &FFrustum::checkSphere);
			frustum.set_function("checkBox", &FFrustum::checkBox);

			auto camera = lua.new_usertype<FCameraComponent>("FCameraComponent", sol::constructors<FCameraComponent()>());
			camera.set("type", &FCameraComponent::type);
			camera.set("frustum", &FCameraComponent::frustum);
			camera.set("fieldOfView", &FCameraComponent::fieldOfView);
			camera.set("near_plane", &FCameraComponent::nearPlane);
			camera.set("far_plane", &FCameraComponent::farPlane);
			camera.set("sensitivity", &FCameraComponent::sensitivity);
			camera.set("xmag", &FCameraComponent::xmag);
			camera.set("ymag", &FCameraComponent::ymag);
			camera.set("view_pos", &FCameraComponent::viewPos);
			camera.set("view", &FCameraComponent::view);
			camera.set("inv_view", &FCameraComponent::invView);
			camera.set("projection", &FCameraComponent::projection);
			camera.set("inv_projection", &FCameraComponent::invProjection);
			camera.set("viewport_dim", &FCameraComponent::viewportDim);
			camera.set("up", &FCameraComponent::up);
			camera.set("forward", &FCameraComponent::forward);
			camera.set("right", &FCameraComponent::right);
			camera.set("active", &FCameraComponent::active);
			camera.set("controllable", &FCameraComponent::controllable);

			auto environment = lua.new_usertype<FEnvironmentComponent>("FEnvironmentComponent", sol::constructors<FEnvironmentComponent()>());
			environment.set("source", &FEnvironmentComponent::source);
			environment.set("cubemap", &FEnvironmentComponent::origin);
			environment.set("active", &FEnvironmentComponent::active);
				
			auto mesh = lua.new_usertype<FMeshComponent>("FMeshComponent", sol::constructors<FMeshComponent()>());

			auto scene = lua.new_usertype<FSceneComponent>("FSceneComponent", sol::constructors<FSceneComponent()>());
			scene.set("source", &FSceneComponent::source);

			auto script = lua.new_usertype<FScriptComponent>("FScriptComponent", sol::constructors<FScriptComponent()>());
			script.set("source", &FScriptComponent::source);

			auto sprite = lua.new_usertype<FSpriteComponent>("FSpriteComponent", sol::constructors<FSpriteComponent()>());
			sprite.set("source", &FSpriteComponent::source);
			sprite.set("image", &FSpriteComponent::image);
			
			auto dir_light = lua.new_usertype<FDirectionalLightComponent>("FDirectionalLightComponent", sol::constructors<FDirectionalLightComponent()>());
			dir_light.set("color", &FDirectionalLightComponent::color);
			dir_light.set("intencity", &FDirectionalLightComponent::intencity);
			dir_light.set("castShadows", &FDirectionalLightComponent::castShadows);
			
			auto point_light = lua.new_usertype<FPointLightComponent>("FPointLightComponent", sol::constructors<FPointLightComponent()>());
			point_light.set("color", &FPointLightComponent::color);
			point_light.set("intencity", &FPointLightComponent::intencity);
			point_light.set("radius", &FPointLightComponent::radius);
			point_light.set("castShadows", &FPointLightComponent::castShadows);
			
			auto spot_light = lua.new_usertype<FSpotLightComponent>("FSpotLightComponent", sol::constructors<FSpotLightComponent()>());
			spot_light.set("color", &FSpotLightComponent::color);
			spot_light.set("target", &FSpotLightComponent::target);
			spot_light.set("intencity", &FSpotLightComponent::intencity);
			spot_light.set("innerAngle", &FSpotLightComponent::innerAngle);
			spot_light.set("outerAngle", &FSpotLightComponent::outerAngle);
			spot_light.set("toTarget", &FSpotLightComponent::toTarget);
			spot_light.set("castShadows", &FSpotLightComponent::castShadows);
		}
	}
}