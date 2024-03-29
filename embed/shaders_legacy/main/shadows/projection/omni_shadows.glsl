
#include "../../lightning_base.glsl"

//Based on https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
#ifndef OMNI_SHADOWS
#define OMNI_SHADOWS

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

float omniShadowProjection(samplerCubeArrayShadow shadowmap_tex, vec3 shadowClip, vec3 offset, float bias, float radius, int layer)
{
	float currentDist = length(shadowClip) / radius;
	return texture(shadowmap_tex, vec4(shadowClip + offset, layer), currentDist - bias);
}

float omniShadowFilterPCF(samplerCubeArrayShadow shadowmap_tex, vec3 shadowClip, float bias, float radius, int layer)
{
	float shadowFactor = 0.0;
	float samples = 4.0;
	float offset  = 0.1;
	for(float x = -offset; x < offset; x += offset / (samples * 0.5))
	{
		for(float y = -offset; y < offset; y += offset / (samples * 0.5))
		{
			for(float z = -offset; z < offset; z += offset / (samples * 0.5))
				shadowFactor += omniShadowProjection(shadowmap_tex, shadowClip, vec3(x, y, z), bias, radius, layer);
		}
	}
	shadowFactor /= (samples * samples * samples);
	return shadowFactor;
}

float getOmniShadow(samplerCubeArrayShadow shadowmap_tex, vec3 fragPos, vec3 lightPos, vec3 N, FOmniShadow light)
{
	float shadow = 1.0;
	vec3 shadowClip = fragPos - lightPos;

	float cosTheta = dot(N, normalize(lightPos - fragPos));
	float bias = 0.1 * tan(acos(cosTheta));
	bias = clamp(bias, 0.0, 0.003);

	bool enablePCF = true;
	if (enablePCF)
		shadow = omniShadowFilterPCF(shadowmap_tex, shadowClip, bias, light.farClip, light.layer);
	else
		shadow = omniShadowProjection(shadowmap_tex, shadowClip, vec3(0.0), bias, light.farClip, light.layer);

	return shadow;
}

#endif