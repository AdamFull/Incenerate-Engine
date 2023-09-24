#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

#include "../shared_shading.glsl"

layout (set = 1, binding = 0) uniform sampler2D depth_tex;
layout (set = 1, binding = 1) uniform sampler2D mrah_tex;
layout (set = 1, binding = 2) uniform sampler2D normal_tex;
layout (set = 1, binding = 3) uniform sampler2D albedo_tex;

//--------------------In/Out locations--------------------
layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragcolor;

//--------------------Uniforms--------------------
layout (std140, set = 0, binding = 0) uniform UBOGeneralMatrices
{
    mat4 projection;
    mat4 invProjection;
	mat4 view;
	mat4 invView;
} data;

layout (std140, set = 0, binding = 1) uniform UBOReflections
{
    float rayStep;
	int iterationCount;
	float distanceBias;
	int sampleCount;
	float samplingCoefficient;
	int debugDraw;
	int isBinarySearchEnabled;
	int isAdaptiveStepEnabled;
	int isExponentialStepEnabled;
	int isSamplingEnabled;
} refl;

vec3 generatePositionFromDepth(vec2 texturePos, float depth) 
{
	vec4 ndc = vec4((texturePos - 0.5) * 2, depth, 1.f);
	vec4 inversed = data.invProjection * ndc;
	inversed /= inversed.w;
	return inversed.xyz;
}

void main()
{
	vec3 albedo = texture(albedo_tex, inUV).xyz;
	vec4 mrah = texture(mrah_tex, inUV);

	float metallic = mrah.g;
	float roughness = mrah.r;

	if(metallic < 0.01)
		outFragcolor = vec4(albedo, 1.0);
	else
	{
		vec3 viewNormal  = (data.view * vec4(texture(normal_tex, inUV).xyz, 0.0)).rgb;
		vec3 viewPos = generatePositionFromDepth(inUV, texture(depth_tex, inUV).x);

		vec3 reflected = normalize(reflect(normalize(viewPos), normalize(viewNormal)));

		vec3 hitPos = viewPos;
		float dDepth;

		vec3 wp = vec3(vec4(viewPos, 1.0) * invView);
		vec3 jitt = mix(vec3(0.0), vec3(hash(wp)), spec);
	}

	

}