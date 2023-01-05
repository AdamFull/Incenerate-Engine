#version 450

//#define DEBUG
#ifdef DEBUG
#define INVOCATION_COUNT 4
#endif

layout (triangles, invocations = INVOCATION_COUNT) in;
layout (triangle_strip, max_vertices = 3) out;

layout (binding = 0) uniform UBOShadowmap 
{
	mat4 viewProjMat[INVOCATION_COUNT];
	int passedLights;
} uboshadow;

void main() 
{
	if(gl_InvocationID > uboshadow.passedLights)
		return;

	for (int i = 0; i < gl_in.length(); i++)
	{
		gl_Layer = gl_InvocationID;
		gl_Position = uboshadow.viewProjMat[gl_InvocationID] * gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}