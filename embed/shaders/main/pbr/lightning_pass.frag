#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_ARB_texture_cube_map_array : enable
#extension GL_GOOGLE_include_directive : require

#define SHADOW_MAP_CASCADE_COUNT 5

//--------------------Includes--------------------
#include "light_models/sascha_williems.glsl"

#include "../shadows/projection/cascade_shadows.glsl"
#include "../shadows/projection/directional_shadows.glsl"
#include "../shadows/projection/omni_shadows.glsl"

#include "../lightning_base.glsl"
#include "../../shader_util.glsl"

//--------------------Texture bindings--------------------
layout (binding = 0) uniform sampler2D brdflut_tex;
layout (binding = 1) uniform samplerCube irradiance_tex;
layout (binding = 2) uniform samplerCube prefiltred_tex;

layout (binding = 3) uniform sampler2D albedo_tex;
layout (binding = 4) uniform sampler2D normal_tex;
layout (binding = 5) uniform sampler2D mrah_tex;
layout (binding = 6) uniform sampler2D emission_tex;
layout (binding = 7) uniform sampler2D depth_tex;
//layout (binding = 8) uniform sampler2D picking_tex;
//layout (binding = 6) uniform sampler2D ssr_tex;

layout (binding = 8) uniform sampler2DArray cascade_shadowmap_tex;
layout (binding = 9) uniform sampler2DArrayShadow direct_shadowmap_tex;
layout (binding = 10) uniform samplerCubeArrayShadow omni_shadowmap_tex;

//--------------------In/Out locations--------------------
layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragcolor;

//--------------------Uniforms--------------------
layout(std140, binding = 12) uniform UBODeferred
{
	mat4 invViewProjection;
	vec4 viewPos;
	int directionalLightCount;
	int spotLightCount;
	int pointLightCount;
} ubo;

//Lights
layout(std430, binding = 15) buffer readonly UBOLights
{
	FDirectionalLight directionalLights[1];
	FSpotLight spotLights[15];
	FPointLight pointLights[16];
} lights;

layout(std140, binding = 13) uniform UBODebug
{
	int mode;
	int cascadeSplit;
	int spotShadowIndex;
	int omniShadowIndex;
	int omniShadowView;
} debug;

#define SHADING_MODEL_CUSTOM

vec3 lightContribution(vec3 albedo, vec3 L, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
{
	return specularContribution(albedo, L, V, N, F0, metallic, roughness);
}

vec3 calculateDirectionalLight(FDirectionalLight light, vec3 worldPosition, vec3 albedo, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
{
	vec3 L = normalize(-light.direction);

	vec3 color = lightContribution(albedo, L, V, N, F0, metallic, roughness);

	float shadow_factor = 1.0;

	if(light.castShadows > 0)
		shadow_factor = getCascadeShadow(cascade_shadowmap_tex, ubo.viewPos.xyz, worldPosition, N, light);

	return light.color * light.intencity * color * shadow_factor;
}

vec3 calculateSpotlight(FSpotLight light, int index, vec3 worldPosition, vec3 albedo, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
{
	vec3 L = normalize(light.position - worldPosition);

	// Direction vector from source to target
	vec3 dir = vec3(normalize(light.direction));
	if(light.toTarget > 0)
		dir = normalize(light.position - light.direction);

	vec3 color = lightContribution(albedo, L, V, N, F0, metallic, roughness);

	// https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_lights_punctual/README.md
	float cd = dot(light.toTarget > 0 ? L : -L, dir);
	float angularAttenuation = clamp(cd * light.lightAngleScale + light.lightAngleOffset, 0.0, 1.0);
	angularAttenuation *= angularAttenuation;	
	
	float shadow_factor = 1.0;

	if(light.castShadows > 0)
		shadow_factor = getDirectionalShadow(direct_shadowmap_tex, worldPosition, N, light, index);

	return light.color * light.intencity * color * angularAttenuation * shadow_factor;
}

vec3 calculatePointLight(FPointLight light, int index, vec3 worldPosition, vec3 albedo, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
{
	vec3 L = normalize(light.position - worldPosition);
	float dist = distance(worldPosition, light.position);
	
	float atten = clamp(1.0 - pow(dist, 2.0f)/pow(light.radius, 2.0f), 0.0f, 1.0f); atten *= atten;
	vec3 color = lightContribution(albedo, L, V, N, F0, metallic, roughness);

	float shadow_factor = 1.0;

	if(light.castShadows > 0)
		shadow_factor = getOmniShadow(omni_shadowmap_tex, worldPosition, ubo.viewPos.xyz, N, light, index, light.radius);

	return light.color * atten * color * light.intencity * shadow_factor;
}

void main() 
{
	// ---Get G-Buffer values---

	//Load depth and world position
	vec2 invUV = vec2(inUV.x, 1-inUV.y);
	float depth = texture(depth_tex, inUV).r;
	vec3 inWorldPos = getPositionFromDepth(invUV, depth, ubo.invViewProjection);

	vec3 normal = texture(normal_tex, inUV).rgb;
	vec3 albedo = texture(albedo_tex, inUV).rgb;
	vec4 mrah = texture(mrah_tex, inUV);

	bool calculateLightning = normal != vec3(0.0f);

	normal = normalize(normal);

	albedo = pow(albedo, vec3(2.2));
	vec3 emission = texture(emission_tex, inUV).rgb;

	float roughness = mrah.r;
	float metallic = mrah.g;
	float occlusion = mrah.b;
	float occlusionStrength = mrah.a;

	vec3 fragcolor = vec3(0.0f);
	if(calculateLightning)
	{
		vec3 cameraPos = ubo.viewPos.xyz;
		// Calculate direction from fragment to viewPosition
		vec3 V = normalize(cameraPos - inWorldPos);
		// Reflection vector
		vec3 R = -normalize(reflect(V, normal));

		vec3 F0 = vec3(0.04f);
		// Reflectance at normal incidence angle
		F0 = mix(F0, albedo, metallic);

		//F0 = vec3(pow(( 1.5 - 1.0) /  (1.5 + 1.0), 2.0));

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
		vec3 ambient = (kD * diffuse + specular);

		ambient = mix(ambient, ambient * occlusion, occlusionStrength);

		// Ambient part
		fragcolor = ambient + (emission * 1.0) + Lo;
	}
	else
	{
		fragcolor = albedo;
	}

	if(debug.mode == 0)
		fragcolor = fragcolor;
	else if(debug.mode == 1)
		fragcolor = inWorldPos;
	else if(debug.mode == 2)
		fragcolor = vec3(depth);
	else if(debug.mode == 3)
		fragcolor = albedo;
	else if(debug.mode == 4)
		fragcolor = normal;
	else if(debug.mode == 5)
		fragcolor = emission;
	else if(debug.mode == 6)
		fragcolor = vec3(roughness);
	else if(debug.mode == 7)
		fragcolor = vec3(metallic);
	else if(debug.mode == 8)
		fragcolor = vec3(occlusion);
	else if(debug.mode == 9)
		fragcolor = vec3(occlusionStrength);
	else if(debug.mode == 10)
		fragcolor = texture(cascade_shadowmap_tex, vec3(inUV, debug.cascadeSplit)).rrr;
	else if(debug.mode == 11)
		fragcolor = texture(direct_shadowmap_tex, vec4(inUV, debug.spotShadowIndex, 0.005)).rrr;
	else if(debug.mode == 12)
		fragcolor = texture(omni_shadowmap_tex, vec4(inUV, debug.omniShadowView, debug.omniShadowIndex), 0.005).rrr;

  	outFragcolor = vec4(fragcolor, 1.0);
}