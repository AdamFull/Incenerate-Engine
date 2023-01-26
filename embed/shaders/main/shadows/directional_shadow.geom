#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout (binding = 0) uniform UBOShadowmap 
{
	mat4 viewProjMat;
	int stride;
} uboshadow;

void main() 
{
	for (int i = 0; i < gl_in.length(); i++)
	{
		gl_Layer = uboshadow.stride;
		gl_Position = uboshadow.viewProjMat * gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}