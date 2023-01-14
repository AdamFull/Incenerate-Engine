
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

float omniShadowProjection(samplerCubeArrayShadow shadomwap_tex, vec3 P, vec3 offset, float bias, int layer)
{
	float currentDist = length(P) / 32.f;
	return texture(shadomwap_tex, vec4(P, layer), currentDist - bias);
}

float omniShadowFilterPCF(samplerCubeArrayShadow shadomwap_tex, vec3 P, float viewDistance, float bias, int layer)
{
	float far_plane = 32.0;
	float shadowFactor = 0.0;
	int samples  = 20;
	float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0; 
	for(int i = 0; i < samples; ++i)
		shadowFactor += omniShadowProjection(shadomwap_tex, P, sampleOffsetDirections[i] * diskRadius, bias, layer);
	shadowFactor /= float(samples); 
	return shadowFactor;
}

float omniShadowFilterPCF(samplerCubeArrayShadow shadomwap_tex, vec3 P, float bias, int layer)
{
	float shadowFactor = 0.0;
	float samples = 4.0;
	float offset  = 0.1;
	for(float x = -offset; x < offset; x += offset / (samples * 0.5))
	{
		for(float y = -offset; y < offset; y += offset / (samples * 0.5))
		{
			for(float z = -offset; z < offset; z += offset / (samples * 0.5))
			{
				shadowFactor += omniShadowProjection(shadomwap_tex, P, vec3(x, y, z), bias, layer);
			}
		}
	}
	shadowFactor /= (samples * samples * samples);
	return shadowFactor;
}

float getOmniShadow(samplerCubeArrayShadow shadomwap_tex, vec3 fragPos, vec3 viewPos, vec3 N, FPointLight light, int layer) 
{
    float shadow = 1.0;
	vec3 shadowClip = fragPos - light.position;	

	//TODO: Fix acne
	float cosTheta = dot(N, normalize(light.position - fragPos));
	float bias = 0.1 * tan(acos(cosTheta));
	bias = clamp(bias, 0.0, 0.003);
	bias = 0.0025;

	bool enablePCF = false;
	if (enablePCF)
		shadow = omniShadowFilterPCF(shadomwap_tex, shadowClip, bias, layer);
	else
		shadow = omniShadowProjection(shadomwap_tex, shadowClip, vec3(0.0), bias, layer);

	return shadow;
}

#endif