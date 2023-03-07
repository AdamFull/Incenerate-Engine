#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec4 inTangent;
layout (location = 5) in vec4 inJoint0;
layout (location = 6) in vec4 inWeight0;

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec3 outColor;
layout(location = 2) out vec3 outNormal;

#include "../../shader_util.glsl"

layout(std140, binding = 0) uniform FUniformData 
{
  	mat4 model;
  	mat4 view;
  	mat4 projection;
  	mat4 normal;
	vec3 viewDir;
	vec2 viewportDim;
	vec4 frustumPlanes[6];
	vec4 object_id;
} data;

void main() 
{
	outUV = inTexCoord;
  	outColor = inColor;

	outNormal = mat3(data.normal) * inNormal;

#ifdef USE_TESSELLATION
	gl_Position = data.model * vec4(inPosition, 1.0);
#else
	gl_Position = data.model * vec4(inPosition, 1.0);
#endif
}