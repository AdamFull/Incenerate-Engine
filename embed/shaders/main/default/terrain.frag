#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

#ifdef HAS_BASECOLORMAP
layout(binding = 2) uniform sampler2D color_tex;
#endif

#ifdef HAS_METALLIC_ROUGHNESS
layout(binding = 3) uniform sampler2D rmah_tex;
#endif

#ifdef HAS_NORMALMAP
layout(binding = 4) uniform sampler2D normal_tex;
#endif

#ifdef HAS_OCCLUSIONMAP
layout(binding = 5) uniform sampler2D occlusion_tex;
#endif

#ifdef HAS_EMISSIVEMAP
layout(binding = 6) uniform sampler2D emissive_tex;
#endif

layout(std140, binding = 0) uniform FUniformData 
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

layout(std140, binding = 19) uniform UBOMaterial
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
	float tessellationStrength;
} material;

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec4 inPosition;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outMRAH;
layout(location = 3) out vec4 outEmissive;
#ifdef EDITOR_MODE
layout(location = 4) out vec4 outObjectID;
#endif

#include "../../shader_util.glsl"

const float minRoughness = 0.04;

//https://github.com/bwasty/gltf-viewer/blob/master/src/shaders/pbr-frag.glsl
void main() 
{
//BASECOLOR
	vec4 albedo_map = vec4(0.0);
#ifdef HAS_BASECOLORMAP
	albedo_map = texture(color_tex, texCoord) * material.baseColorFactor;
#else
	albedo_map = material.baseColorFactor;
#endif
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
	vec4 metalRough = texture(rmah_tex, texCoord);
	roughness = roughness * metalRough.g;
	metallic = metallic * metalRough.b;
#endif
	roughness = clamp(roughness, minRoughness, 1.0);
    metallic = clamp(metallic, 0.01, 1.0);
	pbr_map = vec4(roughness, metallic, 0.0, 0.0);

//NORMALS

	vec3 normal_map = inNormal;
//#ifdef HAS_NORMALMAP
//	normal_map = getTangentSpaceNormalMap(normal_tex, inTBN, texCoord, material.normalScale);
//#else
//	normal_map = inTBN[2].xyz;
//#endif

//AMBIENT OCCLUSION
#ifdef HAS_OCCLUSIONMAP
	pbr_map.b = texture(occlusion_tex, texCoord).r;
	pbr_map.a = material.occlusionStrenth;
#else
	pbr_map.b = 1.0;
	pbr_map.a = 0.0;
#endif

//EMISSION
	vec4 emission = vec4(0.0);
#ifdef HAS_EMISSIVEMAP
    emission = vec4(texture(emissive_tex, texCoord).rgb * material.emissiveFactor, 1.0) * material.emissiveStrength;
#else
	emission = vec4(material.emissiveFactor, 1.0) * material.emissiveStrength;
#endif

	outAlbedo = albedo_map;
	outNormal = vec4(normal_map, 1.0);
	outMRAH = pbr_map;
	outEmissive = emission;
#ifdef EDITOR_MODE
	outObjectID = data.object_id;
#endif
}