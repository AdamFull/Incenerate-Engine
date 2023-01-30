#version 450

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

layout(push_constant) uniform UBODisplayNormal
{
  mat4 model;
  mat4 view;
  mat4 projection;
  mat4 normal;
} debug;

layout (location = 0) in vec3 inNormal[];

layout (location = 0) out vec3 outColor;

const float normalLength = 0.1;

void main(void)
{	
	for(int i=0; i<gl_in.length(); i++)
	{
		vec3 pos = gl_in[i].gl_Position.xyz;
		vec3 normal = normalize(debug.normal * vec4(inNormal[i], 0.0)).xyz;

		gl_Position = debug.projection * debug.view * (debug.model * vec4(pos, 1.0));
		outColor = vec3(1.0, 0.0, 0.0);
		EmitVertex();

		gl_Position = debug.projection * debug.view * (debug.model * vec4(pos + normal * normalLength, 1.0));
		outColor = vec3(0.0, 1.0, 0.0);
		EmitVertex();

		EndPrimitive();
	}
}