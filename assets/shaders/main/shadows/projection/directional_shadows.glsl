#include "../../lightning_base.glsl"
#ifndef DIRECTIONAL_SHADOWS
#define DIRECTIONAL_SHADOWS

float directionalShadowProjection(sampler2DArrayShadow shadomwap_tex, vec4 P, vec2 offset, float bias, int layer)
{
	float shadow = 1.0;
	vec4 shadowCoord = P / P.w;
	shadowCoord.st = shadowCoord.st * 0.5 + 0.5;
	return texture(shadomwap_tex, vec4(shadowCoord.st + offset, layer, shadowCoord.z - bias));
}

float directionalShadowFilterPCF(sampler2DArrayShadow shadomwap_tex, vec4 sc, float bias, int layer)
{
	ivec2 texDim = textureSize(shadomwap_tex, 0).xy;
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += directionalShadowProjection(shadomwap_tex, sc, vec2(dx*x, dy*y), bias, layer);
			count++;
		}
	
	}
	return shadowFactor / count;
}

float getDirectionalShadow(sampler2DArrayShadow shadomwap_tex, vec3 fragpos, vec3 N, FSpotLight light, int layer) 
{
    float shadow = 1.0;
	vec4 shadowClip	= light.shadowView * vec4(fragpos, 1.0);

	float cosTheta = dot(N, normalize(light.position - light.direction));
	float bias = 0.000001 * tan(acos(cosTheta));
	bias = clamp(bias, 0.0, 0.00001);

	bool enablePCF = true;
	if (enablePCF)
		shadow = directionalShadowFilterPCF(shadomwap_tex, shadowClip, bias, layer);
	else
		shadow = directionalShadowProjection(shadomwap_tex, shadowClip, vec2(0.0), bias, layer);
	return shadow;
}

#endif