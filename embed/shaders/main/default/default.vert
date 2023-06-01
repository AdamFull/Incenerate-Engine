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
layout(location = 2) out vec4 outPosition;
layout(location = 3) out mat3 outTBN;

#include "../../shader_util.glsl"
#include "shader_inputs.glsl"

void main() 
{
vec4 localPos;
// Calculate skinned matrix from weights and joint indices of the current vertex
#ifdef HAS_SKIN
	mat4 skinMat = 
		inWeight0.x * jointMatrices[int(inJoint0.x)] +
		inWeight0.y * jointMatrices[int(inJoint0.y)] +
		inWeight0.z * jointMatrices[int(inJoint0.z)] +
		inWeight0.w * jointMatrices[int(inJoint0.w)];

	localPos = meshData.model * skinMat * vec4(inPosition, 1.0);
#else
	localPos = meshData.model * vec4(inPosition, 1.0);
#endif

	outUV = inTexCoord * 1.0;
  	outColor = inColor;

	vec3 normal = mat3(meshData.normal) * inNormal;
	vec4 tangent = vec4(mat3(meshData.normal) * inTangent.xyz, inTangent.w);
	outTBN = calculateTBN(normal, tangent);

#ifdef USE_TESSELLATION
	outPosition = vec4(localPos.xyz / localPos.w, 1.0);
	gl_Position = vec4(inPosition, 1.0);
#else

	outPosition = vec4(localPos.xyz / localPos.w, 1.0);
	gl_Position = data.projection * data.view * outPosition;
#endif
}