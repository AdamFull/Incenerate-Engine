#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout(push_constant) uniform UBOShadowmapModelData
{
	mat4 viewProjMat;
	mat4 model;
	int stride;
} modelData;

void main() 
{
	for (int i = 0; i < gl_in.length(); i++)
	{
		gl_Layer = modelData.stride;
		gl_Position = modelData.viewProjMat * gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}