#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

layout (binding = 0) uniform sampler2D samplerColor;
layout (binding = 1) uniform sampler2D samplerBrightness;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout(std140, binding = 3) uniform FBloomUbo 
{
    //HDR
    float gamma;
	float exposure;
    //Bloom
	float blurScale;
    float blurStrength;
    int direction;
} ubo;

#include "../shared_shaders.glsl"

const float weights[] = float[](
0.0024499299678342, 0.0043538453346397, 0.0073599963704157, 0.0118349786570722, 0.0181026699707781,
0.0263392293891488, 0.0364543006660986, 0.0479932050577658, 0.0601029809166942, 0.0715974486241365,
0.0811305381519717, 0.0874493212267511, 0.0896631113333857, 0.0874493212267511, 0.0811305381519717,
0.0715974486241365, 0.0601029809166942, 0.0479932050577658, 0.0364543006660986, 0.0263392293891488,
0.0181026699707781, 0.0118349786570722, 0.0073599963704157, 0.0043538453346397, 0.0024499299678342);

void main() 
{
    vec2 tex_offset = 1.0 / textureSize(samplerBrightness, 0) * ubo.blurScale; // gets size of single texel
	vec3 result = texture(samplerBrightness, inUV).rgb * weights[0]; // current fragment's contribution

	for(int i = 1; i < weights.length(); ++i)
	{
		if (ubo.direction == 1)
		{
			// H
			result += texture(samplerBrightness, inUV + vec2(tex_offset.x * i, 0.0)).rgb * weights[i] * ubo.blurStrength;
			result += texture(samplerBrightness, inUV - vec2(tex_offset.x * i, 0.0)).rgb * weights[i] * ubo.blurStrength;
		}
		else
		{
			// V
			result += texture(samplerBrightness, inUV + vec2(0.0, tex_offset.y * i)).rgb * weights[i] * ubo.blurStrength;
			result += texture(samplerBrightness, inUV - vec2(0.0, tex_offset.y * i)).rgb * weights[i] * ubo.blurStrength;
		}
	}

	//vec4 result = blur13(samplerBrightness, inUV, textureSize(samplerBrightness, 0), ubo.direction == 1 ? vec2(0.0, 1.0) : vec2(1.0, 0.0), ubo.blurStrength, ubo.blurScale);

    vec3 fragcolor = texture(samplerColor, inUV).rgb + result;
    //Exposure
    fragcolor = vec3(1.0) - exp(-fragcolor * ubo.exposure);
    //Gamma correction
    fragcolor = pow(fragcolor, vec3(1.0 / ubo.gamma));
	outColor = vec4(fragcolor, 1.0);
}