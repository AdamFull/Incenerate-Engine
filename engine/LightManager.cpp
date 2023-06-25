#include "LightManager.h"

using namespace engine;

directionsl_lights_t& CLightManager::getDirectional()
{
	return directionalLights;
}

ecs::FDirectionalLightCommit& CLightManager::getDirectionalLight(size_t index)
{
	return directionalLights.lights[index];
}

spot_lights_t& CLightManager::getSpot()
{
	return spotLights;
}

ecs::FSpotLightCommit& CLightManager::getSpotLight(size_t index)
{
	return spotLights.lights[index];
}

point_lights_t& CLightManager::getPoint()
{
	return pointLights;
}

ecs::FPointLightCommit& CLightManager::getPointLight(size_t index)
{
	return pointLights.lights[index];
}