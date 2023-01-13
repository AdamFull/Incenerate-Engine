#version 460
//Based on https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows

//#define DEBUG
#ifdef DEBUG
#define INVOCATION_COUNT 4
#endif

#define NUM_FACES 6

layout (triangles, invocations = INVOCATION_COUNT) in;
layout (triangle_strip, max_vertices= NUM_FACES * 3) out;

layout (binding = 0) uniform UBOShadowmap 
{
	mat4 viewProjMat[INVOCATION_COUNT * NUM_FACES];
    vec4 lightPos[INVOCATION_COUNT];
	int passedLights;
    float farPlane;
} uboshadow;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outLightPos;

void main() 
{
    if(gl_InvocationID > uboshadow.passedLights)
		return;

    int array_shift = gl_InvocationID * NUM_FACES;
	for(int face = 0; face < NUM_FACES; face++) 
    {
		gl_Layer = face + array_shift;
		for(int vertex_index = 0; vertex_index < 3; vertex_index++) 
        {
            outPosition = gl_in[vertex_index].gl_Position;
            outLightPos = uboshadow.lightPos[gl_InvocationID];
			gl_Position = uboshadow.viewProjMat[gl_Layer] * outPosition;
			EmitVertex();
		}
		EndPrimitive();
	}
}