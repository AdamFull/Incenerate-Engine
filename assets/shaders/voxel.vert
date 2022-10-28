#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

layout (location = 0) out mat4 view_proj;
layout (location = 4) out vec4 vertex_color;

layout (std140, binding = 0) uniform MatricesBlock
{
	mat4 projView;
} matrices;

void main()
{
	vertex_color = color;
	view_proj = matrices.projView;
	gl_Position = vec4(position, 1.0);
}
