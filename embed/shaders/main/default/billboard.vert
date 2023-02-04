#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec4 inTangent;
layout (location = 5) in vec4 inJointIndices;
layout (location = 6) in vec4 inJointWeights;

layout(location = 0) out vec2 fragOffset;
layout(location = 1) out vec2 outUV;

layout(push_constant) uniform UBOBillboard
{
	mat4 projection;
	mat4 view;
	vec3 billboardPosition;
	vec3 color;
	vec4 object_id;
} billboard;

const float LIGHT_RADIUS = 0.5;

void main() 
{
	fragOffset = inPosition.xy;
	outUV = inTexCoord;

	vec3 cameraRight = {billboard.view[0][0], billboard.view[1][0], billboard.view[2][0]};
	vec3 cameraUp = {billboard.view[0][1], billboard.view[1][1], billboard.view[2][1]};

	vec3 worldPos = billboard.billboardPosition + LIGHT_RADIUS * fragOffset.x * cameraRight + LIGHT_RADIUS * fragOffset.y * cameraUp;
	gl_Position = billboard.projection * billboard.view * vec4(worldPos, 1.0);
}