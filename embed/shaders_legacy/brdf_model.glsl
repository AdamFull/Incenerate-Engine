#include "math.glsl"

#ifndef BSDF_MODEL_GLSL
#define BSDF_MODEL_GLSL

float D_GGX(float dotNH, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = dotNH * dotNH * (alpha2 - 1.0f) + 1.0f;
	return (alpha2)/(PI * denom*denom); 
}

float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
	float r = (roughness + 1.0f);
	float k = (r*r) / 8.0f;
	float GL = dotNL / (dotNL * (1.0f - k) + k);
	float GV = dotNV / (dotNV * (1.0f - k) + k);
	return GL * GV;
}

float V_GGX(float dotNL, dotNV, float roughness)
{
	return G_SchlicksmithGGX(dotNL, dotNV, roughness) / (4.f * dotNL * dotNV);
}

vec3 specular_brdf(float dotNH, float dotNL, float dotNV, float roughness)
{
	return vec3(V_GGX(dotNL, dotNV, roughness) * D_GGX(dotNH, roughness));
}

vec3 diffuse_brdf(in vec3 color)
{	
	return (1.f / PI) * color;
}

vec3 conductor_fresnel(in float dotVH, in vec3 f0, in vec3 bsdf)
{
	return bsdf * (f0 + (1.f - f0) * pow(1.f - abs(dotVH), 5));
}

vec3 fresnel_mix(in float dotVH, in float eta, in vec3 base, in vec3 layer)
{
	float a = (1.f - eta) / (1.f + eta);
	vec3 f0 = vec3(a * a);
	vec3 fr = f0 + (1.f - f0) * pow(1.f - abs(dotVH), 5);
	return mix(base, layer, fr);
}

#endif