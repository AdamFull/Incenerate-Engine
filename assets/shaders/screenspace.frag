#version 450

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragcolor;

layout (binding = 0) uniform sampler2D input_tex;

void main()
{
	outFragcolor = texture(input_tex, inUV);
}