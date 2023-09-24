#include "../../lightning_base.glsl"
#ifndef CASCADE_SHADOWS
#define CASCADE_SHADOWS

const bool enableCascadedPCF = true;

const mat4 biasMat = mat4(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0
);

float cascadeShadowProjection(sampler2DArrayShadow shadomwap_tex, vec4 shadowCoord, vec2 offset, uint cascadeIndex, float bias)
{
	return texture(shadomwap_tex, vec4(shadowCoord.st + offset, cascadeIndex, shadowCoord.z - bias));
}

float cascadeShadowFilterPCF(sampler2DArrayShadow shadomwap_tex, vec4 sc, uint cascadeIndex, float bias)
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
			shadowFactor += cascadeShadowProjection(shadomwap_tex, sc, vec2(dx*x, dy*y), cascadeIndex, bias);
			count++;
		}
	}
	return shadowFactor / count;
}

float getCascadeShadow(sampler2DArrayShadow shadomwap_tex, vec3 viewPosition, vec3 worldPosition, vec3 L, vec3 N, vec2 inUV, FCascadeShadow light)
{
	uint cascadeIndex = 0;
	for (uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; ++i)
	{
		if (viewPosition.z < light.cascadeSplits[i])
			cascadeIndex = i + 1;
	}

	const float biasScale = 0.0001f;
	float depth = -viewPosition.z;
	float bias = biasScale * depth;

	vec4 offsetPos = vec4(worldPosition + 0.05 * N, 1.0);
	vec4 shadowCoord = (biasMat * light.cascadeViewProjMat[cascadeIndex]) * offsetPos;
	shadowCoord = shadowCoord / shadowCoord.w;

	float shadow = 1.f;
	if (enableCascadedPCF)
		shadow = cascadeShadowFilterPCF(shadomwap_tex, shadowCoord, cascadeIndex, bias);
	else
		shadow = cascadeShadowProjection(shadomwap_tex, shadowCoord, vec2(0.0), cascadeIndex, bias);

	return shadow;
}

#endif