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

FSpotShadowCommit& CShadowManager::getSpotShadowCommit(size_t index)
{
	return aSpotLightShadows[index];
}

const omni_shadows_t& CShadowManager::getPointLightShadows()
{
	return aPointLightShadows;
}

FOmniShadowCommit& CShadowManager::getPointShadowCommit(size_t index)
{
	return aPointLightShadows[index];
}