#include "../shared_shading.glsl"
#ifndef LIGHTNING_BASE
#define LIGHTNING_BASE

#ifndef MAX_DIRECTIONAL_LIGHTS_COUNT
#define MAX_DIRECTIONAL_LIGHTS_COUNT 1
#endif

struct FDirectionalLight
{
	vec3 direction;
    vec3 color;
	float intencity;
	int shadowIndex;
	int castShadows;
};

#ifdef SHADOW_MAP_CASCADE_COUNT
struct FCascadeShadow
{
	float cascadeSplits[SHADOW_MAP_CASCADE_COUNT];
	mat4 cascadeViewProjMat[SHADOW_MAP_CASCADE_COUNT];
	float farClip;
};
#endif

struct FSpotLight
{
	vec3 position;
    vec3 direction;
    vec3 color;
	float intencity;
	float lightAngleScale; 
    float lightAngleOffset;
	int toTarget;
	int shadowIndex;
	int castShadows;
};

struct FSpotShadow
{
	mat4 shadowView;
	int layer;
};

struct FPointLight
{
	vec3 position;
    vec3 color;
	float intencity;
	float radius;
	int shadowIndex;
	int castShadows;
};

struct FOmniShadow
{
	float farClip;
	int layer;
};

#endif