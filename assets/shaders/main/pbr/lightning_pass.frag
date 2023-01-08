#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_ARB_texture_cube_map_array : enable
#extension GL_GOOGLE_include_directive : require

//--------------------Includes--------------------
#include "light_models/sascha_williems.glsl"

//#include "../shadows/projection/cascade_shadows.glsl"
#include "../shadows/projection/directional_shadows.glsl"
#include "../shadows/projection/omni_shadows.glsl"

#include "../lightning_base.glsl"
#include "../../shader_util.glsl"

//--------------------Texture bindings--------------------
layout (binding = 0) uniform sampler2D brdflut_tex;
layout (binding = 1) uniform samplerCube irradiance_tex;
layout (binding = 2) uniform samplerCube prefiltred_tex;

layout (binding = 3) uniform usampler2D packed_tex;
layout (binding = 4) uniform sampler2D emission_tex;
layout (binding = 5) uniform sampler2D depth_tex;
//layout (binding = 6) uniform sampler2D ssr_tex;

//layout (binding = 7) uniform sampler2DArray cascade_shadowmap_tex;
layout (binding = 8) uniform sampler2DArrayShadow direct_shadowmap_tex;
layout (binding = 9) uniform samplerCubeArrayShadow omni_shadowmap_tex;

//--------------------In/Out locations--------------------
layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragcolor;

//--------------------Uniforms--------------------
layout(std140, binding = 12) uniform UBODeferred
{
	mat4 invViewProjection;
	mat4 invProjMatrix;
	mat4 invViewMatrix;
	mat4 view;
	vec4 viewPos;
	int directionalLightCount;
	int spotLightCount;
	int pointLightCount;
} ubo;

//Lights
layout(std430, binding = 15) buffer UBOLights
{
	FDirectionalLight directionalLights[1];
	FSpotLight spotLights[15];
	FPointLight pointLights[16];
} lights;

#define SHADING_MODEL_CUSTOM

vec3 lightContribution(vec3 albedo, vec3 L, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
{
	return specularContribution(albedo, L, V, N, F0, metallic, roughness);
}

vec3 calculateDirectionalLight(FDirectionalLight light, vec3 worldPosition, vec3 albedo, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
{
	vec3 L = -light.direction;
	float dist = length(L);
	L = normalize(L);
	vec3 color = lightContribution(albedo, L, V, N, F0, metallic, roughness);

	vec3 viewPosition = (ubo.view * vec4(worldPosition, 1.0)).xyz;
	//float shadow_factor = getCascadeShadow(cascade_shadowmap_tex, viewPosition, worldPosition, N, light);
	float shadow_factor = 1.0;

	return light.color * light.intencity * color * shadow_factor;
}

vec3 calculateSpotlight(FSpotLight light, int index, vec3 worldPosition, vec3 albedo, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
{
	vec3 L = light.position - worldPosition;
	float dist = length(L);
	L = normalize(L);

	// Direction vector from source to target
	vec3 dir = vec3(normalize(light.direction));
	if(light.toTarget)
		dir = normalize(light.position - light.direction);
	float cosDir = dot(light.toTarget ? L : -L, dir);
	float spotEffect = smoothstep(light.innerConeAngle, light.outerConeAngle, cosDir);
	float heightAttenuation = smoothstep(100.0, 0.0f, dist);
	vec3 color = lightContribution(albedo, L, V, N, F0, metallic, roughness);
	
	float shadow_factor = getDirectionalShadow(direct_shadowmap_tex, worldPosition, N, light, index);
	//float shadow_factor = 1.0;

	return light.color * light.intencity * color * spotEffect * heightAttenuation * shadow_factor;
}

vec3 calculatePointLight(FPointLight light, int index, vec3 worldPosition, vec3 albedo, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
{
	vec3 L = light.position - worldPosition;
	float dist = length(L);
	L = normalize(L);
	float atten = clamp(1.0 - pow(dist, 2.0f)/pow(light.radius, 2.0f), 0.0f, 1.0f); atten *= atten;
	vec3 color = lightContribution(albedo, L, V, N, F0, metallic, roughness);

	float shadow_factor = getOmniShadow(omni_shadowmap_tex, worldPosition, ubo.viewPos.xyz, N, light, index);
	//float shadow_factor = 1.0;

	return light.color * atten * color * light.intencity * shadow_factor;
}

void main() 
{
	// ---Get G-Buffer values---

	//Load depth and world position
	vec2 invUV = vec2(inUV.x, 1-inUV.y);
	float depth = texture(depth_tex, inUV).r;
	vec3 inWorldPos = getPositionFromDepth(invUV, depth, ubo.invViewProjection);
	//vec3 inWorldPos = WorldPosFromDepth(invUV, depth, ubo.invProjMatrix, ubo.invViewMatrix);

	vec3 normal = vec3(0.0);
	vec3 albedo = vec3(0.0);
	vec4 mrah = vec4(0.0);

	// Loading texture pack
	uvec4 packed_data = texture(packed_tex, inUV);
	unpackTextures(packed_data, normal, albedo, mrah);

	albedo = pow(albedo, vec3(2.2));
	vec3 emission = texture(emission_tex, inUV).rgb;

	float roughness = mrah.r;
	float metallic = mrah.g;
	float occlusion = mrah.b;

	bool calculateLightning = normal != vec3(0.0f);

	vec3 fragcolor = vec3(0.0f);
	if(calculateLightning)
	{
		vec3 cameraPos = ubo.viewPos.xyz;
		// Calculate direction from fragment to viewPosition
		vec3 V = normalize(cameraPos - inWorldPos);
		// Reflection vector
		vec3 R = reflect(-V, normal);

		vec3 F0 = vec3(0.04f);
		// Reflectance at normal incidence angle
		F0 = mix(F0, albedo, metallic);

		// Light contribution
		vec3 Lo = vec3(0.0f);

		//Directional light
		for(int i = 0; i < ubo.directionalLightCount; i++)
		{
			FDirectionalLight light = lights.directionalLights[i];
			Lo += calculateDirectionalLight(light, inWorldPos, albedo, V, normal, F0, metallic, roughness);
		}

		//Spot light
		for(int i = 0; i < ubo.spotLightCount; i++)
		{
			FSpotLight light = lights.spotLights[i];
			Lo += calculateSpotlight(light, i, inWorldPos, albedo, V, normal, F0, metallic, roughness);
		}

		//Point light
		for(int i = 0; i < ubo.pointLightCount; i++) 
		{
			FPointLight light = lights.pointLights[i];
			Lo += calculatePointLight(light, i, inWorldPos, albedo, V, normal, F0, metallic, roughness);
		}

		vec2 brdf = texture(brdflut_tex, vec2(max(dot(normal, V), 0.0f), roughness)).rg;
		vec3 reflection = prefilteredReflection(R, roughness, prefiltred_tex); 
		vec3 irradiance = texture(irradiance_tex, normal).rgb;

		// Diffuse based on irradiance
		vec3 diffuse = albedo * irradiance;

		vec3 F = F_SchlickR(max(dot(normal, V), 0.0f), F0, roughness);
		
		// Specular reflectance
		vec3 specular = reflection * (F * brdf.x + brdf.y);

		// Ambient part
		vec3 kD = 1.0 - F;
		kD *= 1.0 - metallic;	  
		vec3 ambient = (kD * diffuse + specular) * occlusion;

		// Ambient part
		fragcolor = ambient + (emission * 4.0) + Lo;
	}
	else
	{
		fragcolor = albedo;
	}

	//fragcolor = vec3(depth);
	float fxaa_luma = dot(sqrt(fragcolor), vec3(0.299, 0.587, 0.114));
  	outFragcolor = vec4(fragcolor, fxaa_luma);
	//outFragcolor = vec4(albedo, 1.0);
}