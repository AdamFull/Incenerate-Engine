#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec4 inTangent;
layout (location = 5) in vec4 inWeight0;
layout (location = 6) in vec4 inJoint0;

layout(push_constant) uniform UBOShadowmapModelData
{
	mat4 viewProjMat;
	mat4 model;
	vec4 position;
	int stride;
    float farPlane;
} modelData;

void main()
{
	gl_Position = modelData.model * vec4(inPosition, 1.0);
}