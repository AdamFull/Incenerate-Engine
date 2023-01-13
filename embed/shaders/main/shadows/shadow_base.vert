#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec4 inTangent;

layout(push_constant) uniform UBOShadowmapModelData
{
	mat4 model;
} modelData;

void main()
{
	gl_Position = modelData.model * vec4(inPosition, 1.0);
}