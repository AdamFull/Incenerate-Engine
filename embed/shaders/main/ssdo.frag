#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

#define VK_FRAGMENT_SHADER

//--------------------Includes--------------------
#include "../shader_util.glsl"
#include "../math.glsl"

//--------------------Texture bindings--------------------
layout (set = 1, binding = 0) uniform sampler2D albedo_tex;
layout (set = 1, binding = 1) uniform sampler2D normal_tex;
layout (set = 1, binding = 2) uniform sampler2D depth_tex;
layout (set = 1, binding = 3) uniform sampler2D ssdo_noise_tex;

//--------------------In/Out locations--------------------
layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragcolor;

//--------------------Uniforms--------------------
layout (std140, binding = 0) uniform UBODirectionalOcclusion
{
	vec4 samples[SSAO_KERNEL_SIZE];
    mat4 projection;
    mat4 invProjection;
	float zNear;
	float zFar;
	float radius;
	float bias;
} ubo;

const float ssgi_radius = 0.5f;

vec3 get_world_pos_from_depth(vec2 uv, float depth) 
{
    vec4 clipSpacePosition = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewSpacePosition = ubo.invProjection * clipSpacePosition;
    return viewSpacePosition.xyz / viewSpacePosition.w;
}

void main()
{
	vec3 normal = getNormalFromDepth(depth_tex, inUV, ubo.invProjection);
	vec3 albedo = texture(albedo_tex, inUV).rgb;

	vec3 pos = vec3(inUV, texture(depth_tex, inUV).r);
	vec3 NDC_POS = (2.0 * pos) - 1.0;
	vec4 unprojectPosition = ubo.invProjection * vec4(NDC_POS, 1.0);
    vec3 viewPosition = unprojectPosition.xyz / unprojectPosition.w;
	
    // Get a random vector using a noise lookup
	ivec2 texDim = textureSize(depth_tex, 0); 
	ivec2 noiseDim = textureSize(ssdo_noise_tex, 0);
	const vec2 noiseUV = vec2(float(texDim.x)/float(noiseDim.x), float(texDim.y)/(noiseDim.y)) * inUV;  
	vec3 randomVec = texture(ssdo_noise_tex, noiseUV).xyz * 2.0 - 1.0;
	
	// Create TBN matrix
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	// Calculate occlusion value
	vec4 occlusion = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	for(int i = 0; i < SSAO_KERNEL_SIZE; i++)
	{		
		vec3 samplePos = TBN * ubo.samples[i].xyz; 
		samplePos = viewPosition + samplePos * ubo.radius; 
		
		// project
		vec4 offset = vec4(samplePos, 1.0f);
		offset = ubo.projection * offset; 
		offset.xyz /= offset.w; 
		offset.xyz = offset.xyz * 0.5f + 0.5f;
		
		float sampleDepth = -getLinearDepth(depth_tex, offset.xy, ubo.zNear, ubo.zFar);

		float rangeCheck = smoothstep(0.0f, 1.0f, ubo.radius / abs(viewPosition.z - sampleDepth));
		occlusion.rgb += (sampleDepth >= samplePos.z + ubo.bias ? texture(albedo_tex, offset.xy).rgb : albedo) * rangeCheck;           
	}
	//occlusion = 1.0 - (occlusion / float(SSAO_KERNEL_SIZE));
	occlusion.xyz /= float(SSAO_KERNEL_SIZE);

    outFragcolor = occlusion;
}