#include "ShadowManager.h"

using namespace engine;
using namespace engine::ecs;

const cascaded_shadows_t& CShadowManager::getDirectionalLightShadows()
{
	return aDirectionalLightShadows;
}

FCascadedShadowCommit& CShadowManager::getCascadedShadowCommit(size_t index)
{
	return aDirectionalLightShadows[index];
}

const spot_shadows_t& CShadowManager::getSpotLightShadows()
{
	return aSpotLightShadows;
}

ecs::FSpotShadowCommit& CShadowManager::getSpotShadowCommit(size_t index)
{
	return aSpotLightShadows[index];
}