#include "LightsSystem.h"

#include "Engine.h"

#include "ecs/components/TransformComponent.h"
#include "ecs/components/DirectionalLightComponent.h"
#include "ecs/components/SpotLightComponent.h"
#include "ecs/components/PointLightComponent.h"

using namespace engine::ecs;

void CLightsSystem::__create()
{

}

void CLightsSystem::__update(float fDt)
{
	auto& lightsManager = EGEngine->getLights();

	// Collecting directional lights
	{
		auto& directional = lightsManager->getDirectional();
		directional.light_count &= 0ull;

		auto view = registry->view<FTransformComponent, FDirectionalLightComponent>();
		for (auto [entity, transform, light] : view.each())
		{
			if (directional.light_count > MAX_DIRECTIONAL_LIGHT_COUNT)
				break;

			FDirectionalLightCommit commit;
			commit.direction = glm::normalize(glm::toQuat(transform.model) * glm::vec3(0.f, 0.f, 1.f));
			commit.color = light.color;
			commit.intencity = light.intencity;
			commit.shadowIndex = light.shadowIndex;
			commit.castShadows = static_cast<int>(light.castShadows);

			directional.lights[directional.light_count++] = commit;
		}
	}

	// Collecting point lights
	{
		auto& point = lightsManager->getPoint();
		point.light_count &= 0ull;

		auto view = registry->view<FTransformComponent, FPointLightComponent>();
		for (auto [entity, transform, light] : view.each())
		{
			if (point.light_count > MAX_POINT_LIGHT_COUNT)
				break;

			FPointLightCommit commit;
			commit.position = transform.rposition;
			commit.color = light.color;
			commit.intencity = light.intencity;
			commit.radius = light.radius;
			commit.shadowIndex = light.shadowIndex;
			commit.castShadows = static_cast<int>(light.castShadows);

			point.lights[point.light_count++] = commit;
		}
	}

	// Collecting spot lights
	{
		auto& spot = lightsManager->getSpot();
		spot.light_count &= 0ull;

		// TODO: direction as dot product of rotation and some vec
		auto view = registry->view<FTransformComponent, FSpotLightComponent>();
		for (auto [entity, transform, light] : view.each())
		{
			if (spot.light_count > MAX_SPOT_LIGHT_COUNT)
				break;

			FSpotLightCommit commit;
			commit.position = transform.rposition;
			commit.direction = light.toTarget ? light.target : glm::normalize(glm::toQuat(transform.model) * glm::vec3(0.f, 0.f, 1.f));
			commit.color = light.color;
			commit.intencity = light.intencity;
			commit.lightAngleScale = 1.f / glm::max(0.001f, glm::cos(light.innerAngle) - glm::cos(light.outerAngle));
			commit.lightAngleOffset = -glm::cos(light.outerAngle) * commit.lightAngleScale;
			commit.toTarget = static_cast<int>(light.toTarget);
			commit.shadowIndex = light.shadowIndex;
			commit.castShadows = static_cast<int>(light.castShadows);

			spot.lights[spot.light_count++] = commit;
		}
	}
}