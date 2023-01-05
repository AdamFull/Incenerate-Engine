#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

#ifdef HAS_BASECOLORMAP
layout(binding = 1) uniform sampler2D color_tex;
#endif

#ifdef HAS_METALLIC_ROUGHNESS
layout(binding = 2) uniform sampler2D rmah_tex;
#endif

#ifdef HAS_NORMALMAP
layout(binding = 3) uniform sampler2D normal_tex;
#endif

#ifdef HAS_OCCLUSIONMAP
layout(binding = 4) uniform sampler2D occlusion_tex;
#endif

#ifdef HAS_EMISSIVEMAP
layout(binding = 5) uniform sampler2D emissive_tex;
#endif

#ifdef HAS_HEIGHTMAP
layout(binding = 6) uniform sampler2D height_tex;
#endif

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec4 inPosition;
layout(location = 3) in vec4 inOldPosition;
#ifdef HAS_NORMALS
layout(location = 4) in vec3 inNormal;
#endif
#ifdef HAS_TANGENTS
layout (location = 5) in vec4 inTangent;
#endif

layout(location = 0) out uvec4 outPack;
layout(location = 1) out vec4 outEmissive;
layout(location = 2) out vec2 outVelocity;

#include "../../shader_util.glsl"

layout(std140, binding = 0) uniform FUniformData 
{
  	mat4 model;
	mat4 model_old;
  	mat4 view;
  	mat4 projection;
  	mat4 normal;
	vec3 viewDir;
	vec2 viewportDim;
	vec4 frustumPlanes[6];
} data;

layout(std140, binding = 7) uniform UBOMaterial
{
	vec4 baseColorFactor;
	vec3 emissiveFactor;
	float alphaCutoff;
	float normalScale;
	float occlusionStrenth;
	float metallicFactor;
	float roughnessFactor;
	float tessellationFactor;
	float tessellationStrength;
} material;

const float minRoughness = 0.0;

//https://github.com/bwasty/gltf-viewer/blob/master/src/shaders/pbr-frag.glsl
void main() 
{
	vec2 texCoord = inUV;
#ifdef HAS_HEIGHTMAP 
	vec3 viewDir = normalize(data.viewDir - inPosition.xyz);;
	//texCoord = ParallaxMapping(height_tex, inUV, viewDir, 0.01, 8.0, 32.0);
#endif
	float alpha = material.alphaCutoff;
//BASECOLOR
	vec3 albedo_map = vec3(0.0);
#ifdef HAS_BASECOLORMAP
	albedo_map = texture(color_tex, texCoord).rgb * material.baseColorFactor.rgb;
#else
	albedo_map = material.baseColorFactor.rgb;
#endif
	albedo_map *= inColor;

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
    metallic = clamp(metallic, 0.0, 1.0);
	pbr_map = vec4(roughness, metallic, 0.0, 0.0);

//NORMALS
	mat4 normal = data.normal;
#ifndef HAS_TANGENTS
    vec3 pos_dx = dFdx(inPosition.xyz);
    vec3 pos_dy = dFdy(inPosition.xyz);
    vec3 tex_dx = dFdx(vec3(texCoord, 0.0));
    vec3 tex_dy = dFdy(vec3(texCoord, 0.0));
    vec3 t = (tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);

#ifdef HAS_NORMALS
    vec3 ng = normalize(normal * vec4(inNormal, 0.0)).xyz;
#else
    vec3 ng = cross(pos_dx, pos_dy);
#endif
    t = vec3(normal * vec4(normalize(t - ng * dot(ng, t)), 0.0));
    vec3 b = normalize(cross(ng, t));
    mat3 tbn = mat3(t, b, ng);
#else // HAS_TANGENTS
    mat3 tbn = calculateTBN(vec3(normal * vec4(inNormal, 0)), vec4(vec3(normal * vec4(inTangent.xyz, 0.0)), inTangent.w));
#endif //END HAS_TANGENTS

	vec3 normal_map = vec3(0.0);
#ifdef HAS_NORMALMAP
	normal_map = getTangentSpaceNormalMap(normal_tex, tbn, texCoord, material.normalScale);
#else
	normal_map = tbn[2].xyz;
#endif

	//normal_map *= (2.0 * float(gl_FrontFacing) - 1.0);

//AMBIENT OCCLUSION
#ifdef HAS_OCCLUSIONMAP
	pbr_map.b = texture(occlusion_tex, texCoord).r * material.occlusionStrenth;
#endif

//EMISSION
	vec4 emission = vec4(0.0);
#ifdef HAS_EMISSIVEMAP
    emission = vec4(texture(emissive_tex, texCoord).rgb * material.emissiveFactor, 1.0);
#endif

	outPack = packTextures(normal_map, albedo_map, pbr_map);
	outEmissive = emission;

	vec2 newpos = (inPosition.xy / inPosition.w) * 0.5 + 0.5;
    vec2 oldpos = (inOldPosition.xy / inOldPosition.w) * 0.5 + 0.5;
    outVelocity = newpos - oldpos;
}