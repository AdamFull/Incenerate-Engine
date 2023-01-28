#version 450

const vec2 offsets[6] = vec2[](
	vec2(-1.0, -1.0), 
	vec2(-1.0, 1.0), 
	vec2(1.0, -1.0), 
	vec2(1.0, -1.0), 
	vec2(-1.0, 1.0), 
	vec2(1.0, 1.0)
);

layout(location = 0) out vec2 fragOffset;

layout(push_constant) uniform UBOBillboard
{
	mat4 projection;
	mat4 view;
	vec3 cameraPosition;
	vec3 billboardPosition;
} billboard;

const float LIGHT_RADIUS = 1.1;

void main() 
{
	fragOffset = offsets[gl_VertexIndex];

	vec3 cameraRight = {billboard.view[0][0], billboard.view[1][0], billboard.view[2][0]};
	vec3 cameraUp = {billboard.view[0][1], billboard.view[1][1], billboard.view[2][1]};

	vec3 worldPos = billboard.billboardPosition + LIGHT_RADIUS * fragOffset.x * cameraRight + LIGHT_RADIUS * fragOffset.y * cameraUp;
	gl_Position = billboard.projection * billboard.view * vec4(worldPos, 1.0);
}