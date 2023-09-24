#ifndef _SHADER_INPUTS_GLSL
#define _SHADER_INPUTS_GLSL

layout(std140, set = 0, binding = 0) uniform FSkinning
{
	mat4 jointMatrices[128];
};

struct FInstance
{
	mat4 model;
	vec4 color;
};

layout(std430, set = 0, binding = 3) buffer readonly UBOInstancing
{
	FInstance instances[512];
} instancing;

#ifdef BINDLESS_TEXTURES
#define sample_texture(samplerName, texCoord) texture(textures[texture_ids.samplerName], texCoord)
#define sample_texture_lod(samplerName, texCoord, lodn) textureLod(textures[texture_ids.samplerName], texCoord, lodn)
#else
#define sample_texture(samplerName, texCoord) texture(samplerName, texCoord)
#define sample_texture_lod(samplerName, texCoord, lodn) textureLod(samplerName, texCoord, lodn)
#endif

#endif