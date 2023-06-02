#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_ARB_texture_cube_map_array : enable
#extension GL_GOOGLE_include_directive : require

//--------------------Includes--------------------
#include "light_models/sascha_williems.glsl"

#include "../shadows/projection/cascade_shadows.glsl"
#include "../shadows/projection/directional_shadows.glsl"
#include "../shadows/projection/omni_shadows.glsl"

#include "../lightning_base.glsl"
#include "../../shader_util.glsl"

//--------------------Texture bindings--------------------
layout (set = 1, binding = 0) uniform sampler2D brdflut_tex;
layout (set = 1, binding = 1) uniform samplerCube irradiance_tex;
layout (set = 1, binding = 2) uniform samplerCube prefiltred_tex;

layout (set = 1, binding = 3) uniform sampler2D albedo_tex;
layout (set = 1, binding = 4) uniform sampler2D normal_tex;
layout (set = 1, binding = 5) uniform sampler2D mrah_tex;
layout (set = 1, binding = 6) uniform sampler2D emission_tex;
layout (set = 1, binding = 7) uniform sampler2D depth_tex;

layout (set = 1, binding = 8) uniform sampler2D ambient_occlusion_tex;
//layout (binding = 8) uniform sampler2D picking_tex;
//layout (binding = 6) uniform sampler2D ssr_tex;

layout (set = 1, binding = 9) uniform sampler2DArrayShadow cascade_shadowmap_tex;
layout (set = 1, binding = 10) uniform sampler2DArrayShadow direct_shadowmap_tex;
layout (set = 1, binding = 11) uniform samplerCubeArrayShadow omni_shadowmap_tex;

//--------------------In/Out locations--------------------
layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragcolor;

//--------------------Uniforms--------------------
layout(std140, set = 0, binding = 0) uniform UBODeferred
{
	mat4 view;
	mat4 invViewProjection;
	vec4 viewPos;
	int directionalLightCount;
	int spotLightCount;
	int pointLightCount;
	int ambientOcclusion;
} ubo;

//Lights
layout(std430, set = 0, binding = 1) buffer readonly UBOLights
{
	FDirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS_COUNT];
	FSpotLight spotLights[MAX_SPOT_LIGHTS_COUNT];
	FPointLight pointLights[MAX_POINT_LIGHTS_COUNT];
} lights;

layout(std430, set = 0, binding = 2) buffer readonly UBOShadows
{
	FCascadeShadow directionalShadows[MAX_DIRECTIONAL_LIGHT_SHADOW_COUNT];
	FSpotShadow spotShadows[MAX_SPOT_LIGHT_SHADOW_COUNT];
	FOmniShadow pointShadows[MAX_POINT_LIGHT_SHADOW_COUNT];
} shadows;

layout(std140, set = 0, binding = 3) uniform UBODebug
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

	if(light.castShadows > 0 && light.shadowIndex != -1)
		shadow_factor = getCascadeShadow(cascade_shadowmap_tex, (ubo.view * vec4(worldPosition, 1.0)).xyz, worldPosition, L, N, inUV, shadows.directionalShadows[light.shadowIndex]);

	return light.color * light.intencity * color * shadow_factor;
}

vec3 calculateSpotlight(FSpotLight light, vec3 worldPosition, vec3 albedo, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
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

	if(light.castShadows > 0 && light.shadowIndex != -1)
		shadow_factor = getDirectionalShadow(direct_shadowmap_tex, worldPosition, N, normalize(light.direction), shadows.spotShadows[light.shadowIndex]);

	return light.color * light.intencity * color * angularAttenuation * shadow_factor;
}

vec3 calculatePointLight(FPointLight light, int index, vec3 worldPosition, vec3 albedo, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
{
	vec3 L = normalize(light.position - worldPosition);
	float dist = distance(worldPosition, light.position);
	
	float atten = clamp(1.0 - pow(dist, 2.0f)/pow(light.radius, 2.0f), 0.0f, 1.0f); atten *= atten;
	vec3 color = lightContribution(albedo, L, V, N, F0, metallic, roughness);

	float shadow_factor = 1.0;

	if(light.castShadows > 0 && light.shadowIndex != -1)
		shadow_factor = getOmniShadow(omni_shadowmap_tex, worldPosition, light.position, N, shadows.pointShadows[light.shadowIndex]);

	return light.color * atten * color * light.intencity * shadow_factor;
}

void main() 
{
	// ---Get G-Buffer values---

	//Load depth and world position
	float depth = texture(depth_tex, inUV).r;
	vec3 inWorldPos = getPositionFromDepth(inUV, depth, ubo.invViewProjection);

	vec3 normal = texture(normal_tex, inUV).rgb;
	vec3 albedo = texture(albedo_tex, inUV).rgb;
	vec4 mrah = texture(mrah_tex, inUV);

	float ambientOcclusion = texture(ambient_occlusion_tex, inUV).r;

	bool calculateLightning = normal != vec3(0.0f);

	normal = normalize(normal);

	//albedo = pow(albedo, vec3(2.2));
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
			Lo += calculateSpotlight(light, inWorldPos, albedo, V, normal, F0, metallic, roughness);
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
		if(ubo.ambientOcclusion > 0)
			ambient = ambient * ambientOcclusion;

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
	{
		fragcolor = vec3(occlusion);
		if(ubo.ambientOcclusion > 0)
			fragcolor *= texture(ambient_occlusion_tex, inUV).rrr;
	}
	else if(debug.mode == 9)
		fragcolor = vec3(occlusionStrength);
	else if(debug.mode == 10)
	{
	}
	else if(debug.mode == 11)
	{
		vec3 viewPosition = (ubo.view * vec4(inWorldPos, 1.0)).xyz;
		//float viewPosition = length(ubo.viewPos.xyz - inWorldPos);
		float distanceToCamera = viewPosition.z;

		for(int i = 0; i < ubo.directionalLightCount; i++)
		{
			FDirectionalLight light = lights.directionalLights[i];

			if(light.castShadows > 0 && light.shadowIndex != -1)
			{
				FCascadeShadow shadow = shadows.directionalShadows[light.shadowIndex];
				uint cascadeIndex = 0;
				for (uint i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; ++i)
				{
					if (viewPosition.z < shadow.cascadeSplits[i])
						cascadeIndex = i + 1;
				}

				switch(cascadeIndex) 
				{
				case 0 : 
					fragcolor *= vec3(1.0f, 0.25f, 0.25f);
					break;
				case 1 : 
					fragcolor *= vec3(0.25f, 1.0f, 0.25f);
					break;
				case 2 : 
					fragcolor *= vec3(0.25f, 0.25f, 1.0f);
					break;
				case 3 : 
					fragcolor *= vec3(1.0f, 1.0f, 0.25f);
					break;
				case 4 : 
					fragcolor *= vec3(0.25f, 1.0f, 1.0f);
					break;
				case 5 : 
					fragcolor *= vec3(1.0f, 0.25f, 1.0f);
					break;
				case 6 : 
					fragcolor *= vec3(1.0f, 0.5f, 0.5f);
					break;
				case 7 : 
					fragcolor *= vec3(0.5f, 1.0f, 0.5f);
					break;
				default:
					fragcolor *= vec3(1.0f, 1.0f, 1.0f);
					break;
				}
			}
		}
	}
	else if(debug.mode == 12)
		fragcolor = texture(direct_shadowmap_tex, vec4(inUV, debug.spotShadowIndex, 1.0)).rrr;
	else if(debug.mode == 13)
		fragcolor = texture(omni_shadowmap_tex, vec4(inUV, debug.omniShadowView, debug.omniShadowIndex), 1.0).rrr;

  	outFragcolor = vec4(fragcolor, 1.0);
}