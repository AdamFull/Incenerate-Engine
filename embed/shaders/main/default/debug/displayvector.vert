#version 450

layout(push_constant) uniform UBODisplayVector
{
  mat4 model;
  mat4 view;
  mat4 projection;
  vec3 direction;
} debug;

void main(void)
{
	outNormal = inNormal;
	gl_Position = vec4(inPosition.xyz, 1.0);
}