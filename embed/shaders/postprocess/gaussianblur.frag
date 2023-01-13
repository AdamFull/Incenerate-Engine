#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

layout (binding = 0) uniform sampler2D samplerBrightness;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout(push_constant) uniform FBloomUbo 
{
    //Bloom
	float blurScale;
    float blurStrength;
    int direction;
} ubo;

void main() 
{
    vec2 sizeColour = textureSize(samplerBrightness, 0);
	vec4 colour = vec4(0.0f);
	vec2 direction = vec2(ubo.blurStrength, 0.0);
	if(ubo.direction < 0)
		direction = vec2(0.0, ubo.blurStrength);

#if BLUR_TYPE == 5
	vec2 off1 = vec2(1.3333333333333333f) * direction;
	colour += texture(samplerBrightness, inUV) * 0.29411764705882354 * ubo.blurScale;
	colour += texture(samplerBrightness, inUV + (off1 / sizeColour)) * 0.35294117647058826f * ubo.blurScale;
	colour += texture(samplerBrightness, inUV - (off1 / sizeColour)) * 0.35294117647058826f * ubo.blurScale;
#elif BLUR_TYPE == 9
	vec2 off1 = vec2(1.3846153846f) * direction;
	vec2 off2 = vec2(3.2307692308f) * direction;
	colour += texture(samplerBrightness, inUV) * 0.2270270270f * ubo.blurScale;
	colour += texture(samplerBrightness, inUV + (off1 / sizeColour)) * 0.3162162162f * ubo.blurScale;
	colour += texture(samplerBrightness, inUV - (off1 / sizeColour)) * 0.3162162162f * ubo.blurScale;
	colour += texture(samplerBrightness, inUV + (off2 / sizeColour)) * 0.0702702703f * ubo.blurScale;
	colour += texture(samplerBrightness, inUV - (off2 / sizeColour)) * 0.0702702703f * ubo.blurScale;
#elif BLUR_TYPE == 13
	vec2 off1 = vec2(1.411764705882353f) * direction;
	vec2 off2 = vec2(3.2941176470588234f) * direction;
	vec2 off3 = vec2(5.176470588235294f) * direction;
	colour += texture(samplerBrightness, inUV) * 0.1964825501511404f * ubo.blurScale;
	colour += texture(samplerBrightness, inUV + (off1 / sizeColour)) * 0.2969069646728344f * ubo.blurScale;
	colour += texture(samplerBrightness, inUV - (off1 / sizeColour)) * 0.2969069646728344f * ubo.blurScale;
	colour += texture(samplerBrightness, inUV + (off2 / sizeColour)) * 0.09447039785044732f * ubo.blurScale;
	colour += texture(samplerBrightness, inUV - (off2 / sizeColour)) * 0.09447039785044732f * ubo.blurScale;
	colour += texture(samplerBrightness, inUV + (off3 / sizeColour)) * 0.010381362401148057f * ubo.blurScale;
	colour += texture(samplerBrightness, inUV - (off3 / sizeColour)) * 0.010381362401148057f * ubo.blurScale;
#endif
	outColor = colour;
}