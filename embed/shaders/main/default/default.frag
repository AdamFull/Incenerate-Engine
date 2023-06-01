#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_EXT_demote_to_helper_invocation : require
#extension GL_GOOGLE_include_directive : require

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
#include "shader_inputs.glsl"

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
	

#ifndef HAS_TANGENTS
    vec3 pos_dx = dFdx(inPosition.xyz);
    vec3 pos_dy = dFdy(inPosition.xyz);
    vec3 tex_dx = dFdx(vec3(inUV, 0.0));
    vec3 tex_dy = dFdy(vec3(inUV, 0.0));
    vec3 t = (tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);

#ifdef HAS_NORMALS
    vec3 ng = normalize(inTBN[2]);
#else
    vec3 ng = cross(pos_dx, pos_dy);
#endif

    t = normalize(t - ng * dot(ng, t));
    vec3 b = normalize(cross(ng, t));
    mat3 tbn = mat3(t, b, ng);
#else // HAS_TANGENTS
    mat3 tbn = inTBN;
#endif

//NORMALS
	vec4 normal_map = vec4(0.0, 0.0, 0.0, 1.0);
#ifdef HAS_NORMALMAP // HAS_NORMALMAP
	vec4 normal_sample = sample_texture(normal_tex, texCoord);
	normal_map = vec4(getTangentSpaceNormalMap(normal_sample.xyz, tbn, material.normalScale), normal_sample.a);
#else // HAS_NORMALMAP
	normal_map.xyz = tbn[2].xyz;
#endif
	normal_map.a = min(normal_map.a, albedo_map.a);

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
	vec4 emission_sample = sample_texture(emissive_tex, texCoord);
    emission = vec4(emission_sample.rgb * material.emissiveFactor, emission_sample.a) * material.emissiveStrength;
#else // HAS_EMISSIVEMAP
	emission = vec4(material.emissiveFactor, 1.0) * material.emissiveStrength;
#endif // HAS_EMISSIVEMAP

	outAlbedo = albedo_map;
	outNormal = normal_map;
	outMRAH = pbr_map;
	outEmissive = emission;
#ifdef EDITOR_MODE
	outObjectID = meshData.object_id;
#endif
}