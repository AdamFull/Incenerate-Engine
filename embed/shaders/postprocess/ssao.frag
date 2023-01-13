#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (constant_id = 0) const int SSAO_KERNEL_SIZE = 64;
layout (constant_id = 1) const float SSAO_RADIUS = 0.5;

layout(binding = 0) uniform sampler2D samplerPosition;
layout(binding = 1) uniform sampler2D samplerNormal;
layout(binding = 2) uniform sampler2D samplerNoise;

layout(std140, binding = 3) uniform UniformScene 
{
    mat4 projection;
    vec4 cameraPosition;
	vec4 kernel[SSAO_KERNEL_SIZE];
} ubo;

layout (location = 0) in vec2 inUV;

layout (location = 0) out float outFragColor;

void main()
{
    vec3 fragPos = texture(samplerPosition, inUV).rgb;
	vec3 normal = texture(samplerNormal, inUV).rgb;

    // Get a random vector using a noise lookup.
	ivec2 texDim = textureSize(samplerPosition, 0);
	ivec2 noiseDim = textureSize(samplerNoise, 0);
	vec2 noiseUv = vec2(float(texDim.x) / float(noiseDim.x), float(texDim.y) / (noiseDim.y)) * inUV;
	vec3 randomVec = texture(samplerNoise, noiseUv).rgb * 2.0f - 1.0f;

    // Create TBN matrix.
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(tangent, normal);
	mat3 TBN = mat3(tangent, bitangent, normal);

    // Calculate occlusion value
	float occlusion = 0.0f;
	// remove banding
	const float bias = 0.025f;
    for (int i = 0; i < SSAO_KERNEL_SIZE; i++) 
    {
        vec3 samplePos = TBN * ubo.kernel[i].xyz; 
		samplePos = fragPos + samplePos * SSAO_RADIUS;

        // project
		vec4 offset = vec4(samplePos, 1.0f);
		offset = ubo.projection * offset; 
		offset.xyz /= offset.w; 
		offset.xyz = offset.xyz * 0.5f + 0.5f; 

        // Sample depth.
		float sampleDepth = -length(ubo.cameraPosition.xyz - texture(samplerPosition, offset.xy).rgb);
        float rangeCheck = smoothstep(0.0f, 1.0f, SSAO_RADIUS / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0f : 0.0f) * rangeCheck;  
    }
    occlusion = 1.0 - (occlusion / float(SSAO_KERNEL_SIZE));

    outFragColor = occlusion;
}