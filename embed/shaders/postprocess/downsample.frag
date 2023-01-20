#version 450

layout(binding = 0, rgba16f) uniform writeonly image2D writeColour;
layout(binding = 1) uniform sampler2D samplerColour;

layout (location = 0) in vec2 inUV;

#define offset 2

void main()
{
    vec3 color = vec3(0);
    vec2 texSize = textureSize(samplerColour, 0);
    float aspect = texSize.x/texSize.y;

    color += texture(samplerColour, inUV).rgb;
    color += textureOffset(samplerColour, inUV, ivec2(-offset, 0)).rgb;
    color += textureOffset(samplerColour, inUV, ivec2(-offset, offset)).rgb;
    color += textureOffset(samplerColour, inUV, ivec2(0, offset)).rgb;
    color += textureOffset(samplerColour, inUV, ivec2(offset, offset)).rgb;
    color += textureOffset(samplerColour, inUV, ivec2(offset, 0)).rgb;
    color += textureOffset(samplerColour, inUV, ivec2(offset, -offset)).rgb;
    color += textureOffset(samplerColour, inUV, ivec2(0, -offset)).rgb;
    color += textureOffset(samplerColour, inUV, ivec2(-offset, -offset)).rgb;
    color /= 9;

    imageStore(writeColour, ivec2(inUV * imageSize(writeColour)), vec4(color, 1.0));
}