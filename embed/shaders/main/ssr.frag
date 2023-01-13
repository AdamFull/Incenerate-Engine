#version 450
#extension GL_GOOGLE_include_directive : require

//--------------------Includes--------------------
#include "../shader_util.glsl"
#include "../shared_shading.glsl"

//--------------------Texture bindings--------------------
layout (binding = 0) uniform usampler2D packed_tex;
layout (binding = 1) uniform sampler2D depth_tex;

//--------------------In/Out locations--------------------
layout (location = 0) noperspective in vec2 inUV;
layout (location = 0) out vec4 outFragcolor;

//--------------------Uniforms--------------------
layout (std140, binding = 3) uniform UBOReflections
{
	mat4 invViewProjection;
	mat4 viewProjection;
	mat4 projection;
    mat4 invView;
    vec4 camPos;
} ubo;

const float rayStep = 0.1;
const float minRayStep = 0.1;
const float maxSteps = 30;
const float searchDist = 5;
const float searchDistInv = 0.2;
const int numBinarySearchSteps = 5;
const float maxDDepth = 1.0;
const float maxDDepthInv = 1.0;
const float reflectionSpecularFalloffExponent = 3.0;

vec4 GetUV(vec3 hitCoord)
{
	vec4 projectedCoord = ubo.projection * vec4(hitCoord, 1.0);
    projectedCoord.xy /= projectedCoord.w;
    projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
    return projectedCoord;
}

vec3 getPosition(sampler2D testure_sampler, vec2 uv)
{
    float depth = texture(testure_sampler, uv).r;
	return getPositionFromDepth(uv, depth, ubo.invViewProjection);
}

vec3 BinarySearch(vec3 dir, inout vec3 hitCoord, out float dDepth)
{
    float depth;
    for(int i = 0; i < numBinarySearchSteps; i++)
    {
        vec4 projectedCoord = GetUV(hitCoord);
        depth = texture(depth_tex, projectedCoord.xy).r;
        dDepth = hitCoord.z - depth;

        if(dDepth > 0.0)
            hitCoord += dir;

        dir *= 0.5;
        hitCoord -= dir;    
    }

    vec4 projectedCoord = GetUV(hitCoord);
    return vec3(projectedCoord.xy, depth);
}

vec4 RayCast(vec3 dir, inout vec3 hitCoord, out float dDepth)
{
	float depth;
    dir *= rayStep;

    vec4 projectedCoord = vec4(0.0);
    for(int i = 0; i < maxSteps; i++)
    {
        hitCoord += dir;

        projectedCoord = GetUV(hitCoord);
        depth = texture(depth_tex, projectedCoord.xy).r;
        dDepth = hitCoord.z - depth;

        if(dDepth < 0.0)
            return vec4(BinarySearch(dir, hitCoord, dDepth), 1.0);
    }

    return vec4(0.0);
}

void main()
{
    //Load depth and world position;
	vec3 worldPos = getPosition(depth_tex, inUV);

    vec3 normal = vec3(0.0);
	vec3 albedo = vec3(0.0);
	vec4 mrah = vec4(0.0);

	// Loading texture pack
	uvec4 packed_data = texture(packed_tex, inUV);
	unpackTextures(packed_data, normal, albedo, mrah);

    float metallic = mrah.r;
	float roughness = mrah.g;
	float occlusion = mrah.b;
	float height = mrah.a;

    vec3 viewPos = normalize(worldPos - ubo.camPos.xyz);

    // Reflectance at normal incidence angle
    vec3 F0 = vec3(0.04f); 
	F0 = mix(F0, albedo, metallic);
    vec3 F = F_SchlickR(max(dot(normal, viewPos), 0.0f), F0, roughness);

    vec3 reflected = normalize(reflect(viewPos, normalize(normal)));
    vec3 hitPos = viewPos;
    float dDepth;

    vec4 coords = RayCast(reflected, hitPos, dDepth);
    vec2 dCoords = abs(vec2(0.5, 0.5) - coords.xy);
    float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
    float reflectionMultiplier = pow(metallic, reflectionSpecularFalloffExponent) * screenEdgefactor * -reflected.z;

    uvec4 packed_data_2 = texture(packed_tex, dCoords);
	unpackTextures(packed_data_2, normal, albedo, mrah);
    vec3 reflection = albedo * clamp(reflectionMultiplier, 0.0, 0.9) * F;

    outFragcolor = vec4(reflection, 1.0);
}