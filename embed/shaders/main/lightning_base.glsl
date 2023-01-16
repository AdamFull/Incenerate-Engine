#include "../shared_shading.glsl"
#ifndef LIGHTNING_BASE
#define LIGHTNING_BASE

struct FDirectionalLight
{
	vec3 direction;
    vec3 color;
	float intencity;
	bool castShadows;
};

struct FSpotLight
{
	mat4 shadowView;
	vec3 position;
    vec3 direction;
    vec3 color;
	float intencity;
	float lightAngleScale; 
    float lightAngleOffset;
	bool toTarget;
	bool castShadows;
};

struct FPointLight
{
	vec3 position;
    vec3 color;
	float intencity;
	float radius;
	bool castShadows;
};

#endif