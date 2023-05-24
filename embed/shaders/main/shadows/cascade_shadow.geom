#version 450

layout (triangles, invocations = INVOCATION_COUNT) in;
layout (triangle_strip, max_vertices = 3) out;

layout (binding = 0) uniform UBOShadowmap 
{
	mat4 viewProjMat[INVOCATION_COUNT];
} uboshadow;

void main() 
{
	for (int i = 0; i < gl_in.length(); i++)
	{
		gl_Position = uboshadow.viewProjMat[gl_InvocationID] * gl_in[i].gl_Position;
		gl_Layer = gl_InvocationID;
		EmitVertex();
	}
	EndPrimitive();
}