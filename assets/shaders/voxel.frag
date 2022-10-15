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
	vec3 n2 = normalize(geom_data.normal);
	vec3 l2 = normalize(geom_data.l_dir);
	vec3 lighting = vec3(1.0, 0.9, 0.8) * geom_data.color.rgb * max(dot(n2, l2), 0.0);

	vec3 ambient = geom_data.color.rgb * 0.3;

	FragColor = vec4(ambient + lighting, geom_data.color.a);
}
