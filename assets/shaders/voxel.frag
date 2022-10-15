#version 450 core

layout(location = 0) out vec4 FragColor;

struct GeometryData
{
	vec3 normal;
	vec3 position;
	vec4 color;
	vec3 l_dir;
};

layout(location = 0) smooth in GeometryData geom_data;


void main()
{
	vec4 color = vec4(1.f, 1.f, 0.f, 1.f);
	vec3 n2 = normalize(geom_data.normal);
	vec3 l2 = normalize(geom_data.l_dir);
	vec3 lighting = vec3(1.0, 0.9, 0.8) * color.rgb * max(dot(n2, l2), 0.0);

	vec3 ambient =color.rgb * 0.3;

	FragColor = vec4(ambient + lighting, color.a);
}
