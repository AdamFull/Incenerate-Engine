#version 450

layout(binding = 0) uniform sampler2D composition_tex;

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outBrightness;

layout(push_constant) uniform UBOBrightDetect
{
    float bloom_threshold;
} ubo;

void main()
{
    vec3 fragcolor = texture(composition_tex, inUV).rgb;
    float brightness = dot(fragcolor, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > ubo.bloom_threshold)
        outBrightness = vec4(fragcolor, 1.0);
    else
        outBrightness = vec4(0.0, 0.0, 0.0, 1.0);
}