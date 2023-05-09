#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

//#define BINDLESS_TEXTURES
//#define HAS_BASECOLORMAP
//#define HAS_METALLIC_ROUGHNESS
//#define HAS_NORMALMAP
//#define HAS_OCCLUSIONMAP
//#define HAS_EMISSIVEMAP
//#define HAS_HEIGHTMAP

#ifdef BINDLESS_TEXTURES
#extension GL_EXT_nonuniform_qualifier : enable
layout (set = 1, binding = 0) uniform sampler2D textures[];
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


layout(std140, set = 0, binding = 0) uniform FUniformData 
{
  	mat4 model;
  	mat4 view;
  	mat4 projection;
  	mat4 normal;
	vec3 viewDir;
	vec2 viewportDim;
	vec4 frustumPlanes[6];
	vec4 object_id;
} data;

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

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec4 inPosition;
layout(location = 3) in mat3 inTBN;

layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outMRAH;
layout(location = 3) out vec4 outEmissive;
#ifdef EDITOR_MODE
layout(location = 4) out vec4 outObjectID;
#endif

#include "../../shader_util.glsl"

#ifdef BINDLESS_TEXTURES
#define sample_texture(samplerName, texCoord) texture(textures[texture_ids.samplerName], texCoord)
#else
#define sample_texture(samplerName, texCoord) texture(samplerName, texCoord)
#endif

const float minRoughness = 0.04;

//https://github.com/bwasty/gltf-viewer/blob/master/src/shaders/pbr-frag.glsl
void main() 
{
	vec2 texCoord = inUV;

	vec4 albedo_map = vec4(0.0);
#ifdef HAS_BASECOLORMAP
//BASECOLOR
	albedo_map = sample_texture(color_tex, texCoord) * material.baseColorFactor;
#else // HAS_BASECOLORMAP
	albedo_map = material.baseColorFactor;
#endif // HAS_BASECOLORMAP
	albedo_map *= vec4(inColor, 1.0);

	// Alpha modes
	// 0 - opaque
	// 1 - mask
	// 2 - blend

	// Alpha mode mask
	if(material.alphaMode == 1) 
	{
		if(albedo_map.a <= material.alphaCutoff)
			discard;
	}

//METALLIC ROUGHNESS
	vec4 pbr_map = vec4(0.0);
	float roughness = material.roughnessFactor;
	float metallic = material.metallicFactor;
#ifdef HAS_METALLIC_ROUGHNESS
	vec4 metalRough = sample_texture(rmah_tex, texCoord);
	roughness = roughness * metalRough.g;
	metallic = metallic * metalRough.b;
#endif // HAS_METALLIC_ROUGHNESS
	roughness = clamp(roughness, minRoughness, 1.0);
    metallic = clamp(metallic, 0.01, 1.0);
	pbr_map = vec4(roughness, metallic, 0.0, 0.0);

//NORMALS

	vec3 normal_map = vec3(0.0);
#ifdef HAS_NORMALMAP // HAS_NORMALMAP
	normal_map = getTangentSpaceNormalMap(sample_texture(normal_tex, texCoord).rgb, inTBN, material.normalScale);
#else // HAS_NORMALMAP
	normal_map = inTBN[2].xyz;
#endif

//AMBIENT OCCLUSION
#ifdef HAS_OCCLUSIONMAP
	pbr_map.b = sample_texture(occlusion_tex, texCoord).r;
	pbr_map.a = material.occlusionStrenth;
#else // HAS_OCCLUSIONMAP
	pbr_map.b = 1.0;
	pbr_map.a = 0.0;
#endif // HAS_OCCLUSIONMAP

//EMISSION
	vec4 emission = vec4(0.0);
#ifdef HAS_EMISSIVEMAP
    emission = vec4(sample_texture(emissive_tex, texCoord).rgb * material.emissiveFactor, 1.0) * material.emissiveStrength;
#else // HAS_EMISSIVEMAP
	emission = vec4(material.emissiveFactor, 1.0) * material.emissiveStrength;
#endif // HAS_EMISSIVEMAP

	outAlbedo = albedo_map;
	outNormal = vec4(normal_map, 1.0);
	outMRAH = pbr_map;
	outEmissive = emission;
#ifdef EDITOR_MODE
	outObjectID = data.object_id;
#endif
}