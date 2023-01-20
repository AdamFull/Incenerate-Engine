#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

#include "../shader_util.glsl"

#ifndef FXAA_REDUCE_MIN
    #define FXAA_REDUCE_MIN   (1.0/ 128.0)
#endif
#ifndef FXAA_REDUCE_MUL
    #define FXAA_REDUCE_MUL   (1.0 / 8.0)
#endif
#ifndef FXAA_SPAN_MAX
    #define FXAA_SPAN_MAX     8.0
#endif

layout (binding = 0) uniform sampler2D samplerColor;
//layout (binding = 1) uniform sampler2D velocity_tex;
//layout (binding = 2) uniform sampler2D samplerBrightness;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outBrightness;

layout(push_constant) uniform FBloomUbo
{
    bool enableFXAA;
    vec2 texelStep;
    float bloom_threshold;
} ubo;

layout(std140, binding = 3) uniform UBOFXAA
{
	float qualitySubpix;
	float qualityEdgeThreshold;
	float qualityEdgeThresholdMin;
} fxaa_p;

layout(std140, binding = 4) uniform UBOTonemap
{
	float gamma;
	float exposure;
} tonemap;

vec2 flippedUV(vec2 uv)
{
    return vec2(uv.s, uv.t * -1.0);
}

void main() 
{
    vec3 fragcolor = vec3(0.0);
    if(ubo.enableFXAA)
        fragcolor = fxaa(samplerColor, inUV, fxaa_p.qualitySubpix, FXAA_REDUCE_MIN, FXAA_REDUCE_MUL, FXAA_SPAN_MAX, ubo.texelStep).rgb;
    else
        fragcolor = texture(samplerColor, inUV).rgb;

    //Exposure
    fragcolor = Uncharted2Tonemap(fragcolor * tonemap.exposure);
    //Gamma correction
    fragcolor = fragcolor * (1.0f / Uncharted2Tonemap(vec3(11.2f)));
    fragcolor = pow(fragcolor, vec3(1.0f / tonemap.gamma));
    
	outColor = vec4(fragcolor, 1.0);

    float brightness = dot(outColor.xyz, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > ubo.bloom_threshold)
        outBrightness = vec4(fragcolor, 1.0);
    else
        outBrightness = vec4(0.0, 0.0, 0.0, 1.0);
}