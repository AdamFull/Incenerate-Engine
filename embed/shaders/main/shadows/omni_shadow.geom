#version 460
//Based on https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows

#define NUM_FACES 6

layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

layout(push_constant) uniform UBOShadowmapModelData
{
	mat4 viewProjMat;
	mat4 model;
	vec4 position;
	int stride;
    float farPlane;
} modelData;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outLightPos;

void main() 
{
	for(int vertex_index = 0; vertex_index < gl_in.length(); vertex_index++) 
    {	
		gl_Layer = modelData.stride;
        outPosition = gl_in[vertex_index].gl_Position;
        outLightPos = modelData.position;
		gl_Position = modelData.viewProjMat * outPosition;
		EmitVertex();
	}
	EndPrimitive();
}