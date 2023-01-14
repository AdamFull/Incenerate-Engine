#pragma once

#include "components/CameraComponent.h"
#include "components/EnvironmentComponent.h"

namespace engine
{
	namespace ecs
	{
		static entt::entity get_active_camera(const entt::registry& registry)
		{
			auto view = registry.view<FCameraComponent>();
			for (auto [entity, camera] : view.each())
				if (camera.active)
					return entity;
			return entt::null;
		}

		static entt::entity get_active_skybox(const entt::registry& registry)
		{
			auto view = registry.view<FEnvironmentComponent>();
			for (auto [entity, skybox] : view.each())
				if(skybox.active && skybox.loaded)
					return entity;
			return entt::null;
		}

		static void set_active_skybox(entt::registry& registry, const entt::entity& target)
		{
			auto view = registry.view<FEnvironmentComponent>();
			for (auto [entity, skybox] : view.each())
				if (skybox.active && entity != target)
					skybox.active = false;
		}
	}
}