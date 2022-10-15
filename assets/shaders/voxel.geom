#version 450 core

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

struct GeometryData
{
	vec3 normal;
	vec3 position;
	vec4 color;
	vec3 l_dir;
};

layout(location = 0) in mat4 view_proj[];
layout(location = 4) in vec4 vertex_color[];

layout(location = 0) smooth out GeometryData geom_data;

const float face_size = 0.25;

void push_vertex(in mat4 mat, in vec4 point, in vec3 pos, in vec3 normal, in vec4 color)
{
	gl_Position = mat * (point + vec4(pos, 1.0));

	geom_data.normal = normalize(normal);
	geom_data.position = gl_Position.xyz;
	geom_data.color = color;
	geom_data.l_dir = normalize(vec3(1000.0) - geom_data.position);

	EmitVertex();
}

void main()
{
	mat4 mat = view_proj[0];
	vec4 point = gl_in[0].gl_Position;
	vec4 color = vertex_color[0];

	float half_size = face_size * 0.5;

	////////////
	// front
	////////////

	push_vertex(mat, point, vec3(-half_size, -half_size, half_size), vec3(0, 0, 1), color);
	push_vertex(mat, point, vec3(half_size, -half_size, half_size), vec3(0, 0, 1), color);
	push_vertex(mat, point, vec3(-half_size, half_size, half_size), vec3(0, 0, 1), color);
	push_vertex(mat, point, vec3(half_size, half_size, half_size), vec3(0, 0, 1), color);

	EndPrimitive();

	////////////
	// top
	////////////

	push_vertex(mat, point, vec3(-half_size, half_size, half_size), vec3(0, 1, 0), color);
	push_vertex(mat, point, vec3(half_size, half_size, half_size), vec3(0, 1, 0), color);
	push_vertex(mat, point, vec3(-half_size, half_size, -half_size), vec3(0, 1, 0), color);
	push_vertex(mat, point, vec3(half_size, half_size, -half_size), vec3(0, 1, 0), color);

	EndPrimitive();

	////////////
	// back
	////////////

	push_vertex(mat, point, vec3(half_size, -half_size, -half_size), vec3(0, 0, -1), color);
	push_vertex(mat, point, vec3(-half_size, -half_size, -half_size), vec3(0, 0, -1), color);
	push_vertex(mat, point, vec3(half_size, half_size, -half_size), vec3(0, 0, -1), color);
	push_vertex(mat, point, vec3(-half_size, half_size, -half_size), vec3(0, 0, -1), color);

	EndPrimitive();

	////////////
	// bottom
	////////////

	push_vertex(mat, point, vec3(-half_size, -half_size, -half_size), vec3(0, -1, 0), color);
	push_vertex(mat, point, vec3(half_size, -half_size, -half_size), vec3(0, -1, 0), color);
	push_vertex(mat, point, vec3(-half_size, -half_size, half_size), vec3(0, -1, 0), color);
	push_vertex(mat, point, vec3(half_size, -half_size, half_size), vec3(0, -1, 0), color);

	EndPrimitive();

	////////////
	// left
	////////////

	push_vertex(mat, point, vec3(-half_size, -half_size, -half_size), vec3(-1, 0, 0), color);
	push_vertex(mat, point, vec3(-half_size, -half_size, half_size), vec3(-1, 0, 0), color);
	push_vertex(mat, point, vec3(-half_size, half_size, -half_size), vec3(-1, 0, 0), color);
	push_vertex(mat, point, vec3(-half_size, half_size, half_size), vec3(-1, 0, 0), color);

	EndPrimitive();

	////////////
	// right
	////////////

	push_vertex(mat, point, vec3(half_size, -half_size, half_size), vec3(1, 0, 0), color);
	push_vertex(mat, point, vec3(half_size, -half_size, -half_size), vec3(1, 0, 0), color);
	push_vertex(mat, point, vec3(half_size, half_size, half_size), vec3(1, 0, 0), color);
	push_vertex(mat, point, vec3(half_size, half_size, -half_size), vec3(1, 0, 0), color);

	EndPrimitive();
}