#pragma once

#include "ecs/components/DirectionalLightComponent.h"
#include "ecs/components/PointLightComponent.h"
#include "ecs/components/SpotLightComponent.h"

namespace engine
{
	template<class _LightType, size_t _LightCount>
	struct FLightCommitData
	{
		uint32_t light_count{ 0 };
		std::array<_LightType, _LightCount> lights;
	};

	using directionsl_lights_t = FLightCommitData<ecs::FDirectionalLightCommit, MAX_DIRECTIONAL_LIGHT_COUNT>;
	using spot_lights_t = FLightCommitData<ecs::FSpotLightCommit, MAX_SPOT_LIGHT_COUNT>;
	using point_lights_t = FLightCommitData<ecs::FPointLightCommit, MAX_POINT_LIGHT_COUNT>;

	class CLightManager
	{
	public:
		directionsl_lights_t& getDirectional();
		ecs::FDirectionalLightCommit& getDirectionalLight(size_t index);

		spot_lights_t& getSpot();
		ecs::FSpotLightCommit& getSpotLight(size_t index);

		point_lights_t& getPoint();
		ecs::FPointLightCommit& getPointLight(size_t index);
	private:
		directionsl_lights_t directionalLights{};
		spot_lights_t spotLights{};
		point_lights_t pointLights{};
	};
}