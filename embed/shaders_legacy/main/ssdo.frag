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

void main()
{
	float bias = ubo.bias;
	outFragcolor = texture(albedo_tex, inUV);
}