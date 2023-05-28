#pragma once

#include "ecs/components/DirectionalLightComponent.h"
#include "ecs/components/PointLightComponent.h"
#include "ecs/components/SpotLightComponent.h"

namespace engine
{
	using cascaded_shadows_t = std::array<ecs::FCascadedShadowCommit, MAX_DIRECTIONAL_LIGHT_SHADOW_COUNT>;
	using spot_shadows_t = std::array<ecs::FSpotShadowCommit, MAX_SPOT_LIGHT_SHADOW_COUNT>;

	class CShadowManager
	{
	public:
		const cascaded_shadows_t& getDirectionalLightShadows();
		ecs::FCascadedShadowCommit& getCascadedShadowCommit(size_t index);

		const spot_shadows_t& getSpotLightShadows();
		ecs::FSpotShadowCommit& getSpotShadowCommit(size_t index);
	private:
		cascaded_shadows_t aDirectionalLightShadows{};
		spot_shadows_t aSpotLightShadows{};
	};
}