#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

layout(std140, binding = 0) uniform FUniformData 
{
	mat4 view;
  	mat4 projection;
} ubo;

void main()
{
	vec4 pos = vec4(aPos.x, aPos.y, aPos.z, 1.0);
   gl_Position = ubo.projection * ubo.view * pos;
}