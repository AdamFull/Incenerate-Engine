#version 460
//Based on https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows

#define NUM_FACES 6

layout (triangles) in;
layout (triangle_strip, max_vertices= NUM_FACES * 3) out;

layout (binding = 0) uniform UBOShadowmap 
{
	mat4 viewProjMat;
	vec4 position;
	int stride;
    float farPlane;
} uboshadow;

layout(push_constant) uniform UBOShadowmapModelData
{
	mat4 model;
	int stride;
} modelData;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outLightPos;

void main() 
{
	for(int vertex_index = 0; vertex_index < 3; vertex_index++) 
    {	
		gl_Layer = modelData.stride;
        outPosition = gl_in[vertex_index].gl_Position;
        outLightPos = uboshadow.position;
		gl_Position = uboshadow.viewProjMat * outPosition;
		EmitVertex();
	}
	EndPrimitive();
}