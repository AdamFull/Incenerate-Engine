#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec4 inTangent;
layout (location = 5) in vec4 inJointIndices;
layout (location = 6) in vec4 inJointWeights;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec4 outTangent;

void main(void)
{
	outNormal = inNormal;
	outTangent = inTangent;
	gl_Position = vec4(inPosition.xyz, 1.0);
}