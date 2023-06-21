#ifndef _SHADER_INPUTS_GLSL
#define _SHADER_INPUTS_GLSL

//#ifdef BINDLESS_TEXTURES
//#extension GL_EXT_nonuniform_qualifier : enable
//layout(set = 1, binding = 0) uniform sampler2D textures[];
//#else
//layout(set = 1, binding = 0) uniform sampler2D color_tex;
//#endif

layout(std140, set = 0, binding = 0) uniform FSkinning
{
	mat4 jointMatrices[128];
};

//layout(std140, set = 0, binding = 1) uniform UBOMaterial
//{
//	vec4 baseColorFactor;
//	vec3 emissiveFactor;
//	float emissiveStrength;
//	int alphaMode;
//	float alphaCutoff;
//	float normalScale;
//	float occlusionStrenth;
//	float metallicFactor;
//	float roughnessFactor;
//	float tessellationFactor;
//	float displacementStrength;
//} material;

//#ifdef BINDLESS_TEXTURES
//layout(std140, set = 0, binding = 2) uniform UBOMaterialTextures
//{
//	uint color_tex;
//	uint rmah_tex;
//	uint normal_tex;
//	uint occlusion_tex;
//	uint emissive_tex;
//	uint height_tex;
//} texture_ids;
//#endif

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