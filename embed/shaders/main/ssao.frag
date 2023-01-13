#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

//--------------------Includes--------------------
#include "../shader_util.glsl"

//#define DEBUG
#ifdef DEBUG
#define SSAO_KERNEL_SIZE 32
#define SSAO_RADIUS 0.3
#endif

//--------------------Texture bindings--------------------
layout (binding = 0) uniform usampler2D packed_tex;
layout (binding = 1) uniform sampler2D depth_tex;
layout (binding = 2) uniform sampler2D ssao_noise_tex;

//--------------------In/Out locations--------------------
layout (location = 0) in vec2 inUV;
layout (location = 0) out float outFragcolor;

//--------------------Uniforms--------------------
layout (std140, binding = 3) uniform UBOSSAOKernel
{
	vec4 samples[SSAO_KERNEL_SIZE];
    mat4 invViewProjection;
    mat4 projection;
} ubo;

vec3 getPosition(sampler2D testure_sampler, vec2 uv)
{
    float depth = texture(testure_sampler, uv).r;
	return getPositionFromDepth(uv, depth, ubo.invViewProjection);
}

void main()
{
    // Get G-Buffer values
    //Load depth and world position
	vec3 fragPos = getPosition(depth_tex, inUV);

	vec3 normal = vec3(0.0);
	vec3 albedo = vec3(0.0);
	vec4 mrah = vec4(0.0);

	// Loading texture pack
	uvec4 packed_data = texture(packed_tex, inUV);
	unpackTextures(packed_data, normal, albedo, mrah);

    // Get a random vector using a noise lookup
	ivec2 texDim = textureSize(depth_tex, 0); 
	ivec2 noiseDim = textureSize(ssao_noise_tex, 0);
	const vec2 noiseUV = vec2(float(texDim.x)/float(noiseDim.x), float(texDim.y)/(noiseDim.y)) * inUV;  
	vec3 randomVec = texture(ssao_noise_tex, noiseUV).xyz;
	
	// Create TBN matrix
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	// Calculate occlusion value
	float occlusion = 0.0f;
	// remove banding
	const float bias = 0.025f;
	for(int i = 0; i < SSAO_KERNEL_SIZE; i++)
	{		
		vec3 samplePos = TBN * ubo.samples[i].xyz; 
		samplePos = fragPos + samplePos * SSAO_RADIUS; 
		
		// project
		vec4 offset = vec4(samplePos, 1.0f);
		offset = ubo.projection * offset; 
		offset.xyz /= offset.w; 
		offset.xyz = offset.xyz * 0.5f + 0.5f;
		
		float sampleDepth = -texture(depth_tex, offset.xy).r;

		float rangeCheck = smoothstep(0.0f, 1.0f, SSAO_RADIUS / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0f : 0.0f) * rangeCheck;           
	}
	occlusion = 1.0 - (occlusion / float(SSAO_KERNEL_SIZE));

    outFragcolor = occlusion;
}