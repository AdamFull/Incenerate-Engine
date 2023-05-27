#include "../../lightning_base.glsl"
#ifndef CASCADE_SHADOWS
#define CASCADE_SHADOWS

//const mat4 biasMat = mat4( 
//	0.5, 0.0, 0.0, 0.0,
//	0.0, 0.5, 0.0, 0.0,
//	0.0, 0.0, 0.5, 0.0,
//	0.5, 0.5, 0.5, 1.0
//);

const mat4 biasMat = mat4(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0
);

float cassadeShadowProjection(sampler2DArray shadomwap_tex, vec4 shadowCoord, vec2 offset, uint cascadeIndex, float bias)
{
	float shadow = 1.0;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) 
	{
		float dist = texture(shadomwap_tex, vec3(shadowCoord.st + offset, cascadeIndex)).r;
		if (shadowCoord.w > 0 && dist < shadowCoord.z - bias) {
			shadow = 0.0;
		}
	}
	return shadow;
}

float cascadeShadowFilterPCF(sampler2DArray shadomwap_tex, vec4 sc, uint cascadeIndex, float bias)
{
	ivec2 texDim = textureSize(shadomwap_tex, 0).xy;
	float scale = 0.75;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++) 
	{
		for (int y = -range; y <= range; y++) 
		{
			shadowFactor += cassadeShadowProjection(shadomwap_tex, sc, vec2(dx*x, dy*y), cascadeIndex, bias);
			count++;
		}
	}
	return shadowFactor / count;
}

float getShadowCascadeProjection(sampler2DArray shadomwap_tex, vec3 worldPosition, uint cascadeIndex, vec3 L, vec3 N, FDirectionalLight light, bool filtering)
{
	//const float normalBias = 0.05;
	float bias = max(0.05 * (1.0 - dot(N, L)), 0.005);
	const float biasModifier = 0.5f;
	if (cascadeIndex == SHADOW_MAP_CASCADE_COUNT - 1)
		bias *= 1.f / (light.farClip * biasModifier);
	else
		bias *= 1.f / (abs(light.cascadeSplits[cascadeIndex]) * biasModifier);

	//vec3 offsetPosition = worldPosition + N * normalBias;
	vec4 shadowCoord = (biasMat * light.cascadeViewProjMat[cascadeIndex]) * vec4(worldPosition, 1.0);
	shadowCoord = shadowCoord / shadowCoord.w;

	float shadow = 1.f;
	if (filtering)
		shadow = cascadeShadowFilterPCF(shadomwap_tex, shadowCoord, cascadeIndex, bias);
	else
		shadow = cassadeShadowProjection(shadomwap_tex, shadowCoord, vec2(0.0), cascadeIndex, bias);

	return shadow;
}

float getCascadeShadow(sampler2DArray shadomwap_tex, vec3 viewPosition, vec3 worldPosition, vec3 L, vec3 N, FDirectionalLight light)
{
	bool enablePCF = true;

	uint cascadeIndex = 0;
	for (uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; ++i)
	{
		if (viewPosition.z < light.cascadeSplits[i])
			cascadeIndex = i + 1;
	}

	float shadow = getShadowCascadeProjection(shadomwap_tex, worldPosition, cascadeIndex, L, N, light, enablePCF);

	// Blend with next cascade
	if (cascadeIndex < SHADOW_MAP_CASCADE_COUNT - 1) 
	{
		float shadowNext = getShadowCascadeProjection(shadomwap_tex, worldPosition, cascadeIndex + 1, L, N, light, enablePCF);
	
		float blendFactor = smoothstep(light.cascadeSplits[cascadeIndex] - 20.1f, light.cascadeSplits[cascadeIndex], viewPosition.z);
		shadow = mix(shadow, shadowNext, blendFactor);
	}

	return shadow;
}

#endif