#version 450

layout(binding = 0) uniform sampler2D downsample_tex;

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outColor;

#define offset 5

void main()
{
    vec3 color = vec3(0);
    vec2 texSize = textureSize(downsample_tex, 0);
    float aspect = texSize.x/texSize.y;

    color += texture(downsample_tex, inUV).rgb;
    color += textureOffset(downsample_tex, inUV, ivec2(-offset, 0)).rgb;
    color += textureOffset(downsample_tex, inUV, ivec2(-offset, offset)).rgb;
    color += textureOffset(downsample_tex, inUV, ivec2(0, offset)).rgb;
    color += textureOffset(downsample_tex, inUV, ivec2(offset, offset)).rgb;
    color += textureOffset(downsample_tex, inUV, ivec2(offset, 0)).rgb;
    color += textureOffset(downsample_tex, inUV, ivec2(offset, -offset)).rgb;
    color += textureOffset(downsample_tex, inUV, ivec2(0, -offset)).rgb;
    color += textureOffset(downsample_tex, inUV, ivec2(-offset, -offset)).rgb;
    color /= 9;
    outColor = vec4(color, 1.0);
}