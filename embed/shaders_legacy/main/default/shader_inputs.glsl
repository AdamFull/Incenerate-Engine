#ifndef _SHADER_INPUTS_GLSL
#define _SHADER_INPUTS_GLSL

#ifdef BINDLESS_TEXTURES
#extension GL_EXT_nonuniform_qualifier : enable
layout(set = 1, binding = 0) uniform sampler2D textures[];
#else
#ifdef HAS_BASECOLORMAP
layout(set = 1, binding = 0) uniform sampler2D color_tex;
#endif // HAS_BASECOLORMAP

#ifdef HAS_METALLIC_ROUGHNESS
layout(set = 1, binding = 1) uniform sampler2D rmah_tex;
#endif // HAS_METALLIC_ROUGHNESS

#ifdef HAS_NORMALMAP
layout(set = 1, binding = 2) uniform sampler2D normal_tex;
#endif // HAS_NORMALMAP

#ifdef HAS_OCCLUSIONMAP
layout(set = 1, binding = 3) uniform sampler2D occlusion_tex;
#endif // HAS_OCCLUSIONMAP

#ifdef HAS_EMISSIVEMAP
layout(set = 1, binding = 4) uniform sampler2D emissive_tex;
#endif // HAS_OCCLUSIONMAP

#ifdef HAS_HEIGHTMAP
layout(set = 1, binding = 5) uniform sampler2D height_tex;
#endif // HAS_HEIGHTMAP
#endif // BINDLESS_TEXTURES

layout(push_constant) uniform UBOMeshData
{
	mat4 model;
	mat4 normal;
	vec4 object_id;
} meshData;

layout(std140, set = 0, binding = 0) uniform FUniformData
{
	mat4 view;
	mat4 projection;
	vec3 viewDir;
	vec2 viewportDim;
	vec4 frustumPlanes[6];
} data;

#ifdef HAS_SKIN
layout(std140, set = 0, binding = 1) uniform FSkinning
{
	mat4 jointMatrices[128];
};
#endif

layout(std140, set = 0, binding = 2) uniform UBOMaterial
{
	vec4 baseColorFactor;
	vec3 emissiveFactor;
	float emissiveStrength;
	int alphaMode;
	float alphaCutoff;
	float normalScale;
	float occlusionStrenth;
	float metallicFactor;
	float roughnessFactor;
	float tessellationFactor;
	float displacementStrength;
} material;

#ifdef BINDLESS_TEXTURES
layout(std140, set = 0, binding = 3) uniform UBOMaterialTextures
{
	uint color_tex;
	uint rmah_tex;
	uint normal_tex;
	uint occlusion_tex;
	uint emissive_tex;
	uint height_tex;
} texture_ids;
#endif

struct FInstance
{
	mat4 model;
	vec4 color;
};

layout(std430, set = 0, binding = 4) buffer readonly UBOInstancing
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