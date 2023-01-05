#include "../shared_shading.glsl"
#ifndef LIGHTNING_BASE
#define LIGHTNING_BASE

struct FDirectionalLight
{
	float cascadeSplits[SHADOW_MAP_CASCADE_COUNT];
	mat4 cascadeViewProjMat[SHADOW_MAP_CASCADE_COUNT];
	vec3 direction;
    vec3 color;
	float intencity;
};

struct FSpotLight
{
	mat4 shadowView;
	vec3 position;
    vec3 direction;
    vec3 color;
	float intencity;
	float innerConeAngle;
    float outerConeAngle;
	bool toTarget;
};

struct FPointLight
{
	vec3 position;
    vec3 color;
	float intencity;
	float radius;
};

#endif