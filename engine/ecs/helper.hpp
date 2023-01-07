#pragma once

#include "components/CameraComponent.h"
#include "components/SkyboxComponent.h"

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
			auto view = registry.view<FSkyboxComponent>();
			for (auto [entity, skybox] : view.each())
					return entity;
			return entt::null;
		}
	}
}