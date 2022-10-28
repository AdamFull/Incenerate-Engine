#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

layout (std140, binding = 0) uniform MatricesBlock
{
	mat4 projView;
} matrices;

void main()
{
	vec4 pos = vec4(aPos.x, aPos.y, aPos.z, 1.0);
   gl_Position = matrices.projView * pos;
}