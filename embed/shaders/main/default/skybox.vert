#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec4 inTangent;
layout (location = 5) in vec4 inWeight0;
layout (location = 6) in vec4 inJoint0;

//layout(binding = 0) uniform FUniformData 
//{
//	mat4 model;
//  	mat4 view;
//  	mat4 projection;
//  	mat4 normal;
//	vec3 viewDir;
//	vec2 viewportDim;
//	vec4 frustumPlanes[6];
//	vec4 object_id;
//} ubo;

//layout (location = 0) out vec3 outWorldPos;
layout (location = 0) out vec2 outUV;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() 
{
	//outWorldPos = inPosition;
	//mat4 view = ubo.view;
	//view[3] = vec4(0.0, 0.0, 0.0, 1.0);
	//gl_Position = ubo.projection * view * vec4(inPosition, 1.0);

	outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(outUV * 2.0f - 1.0f, 0.0f, 1.0f);
}