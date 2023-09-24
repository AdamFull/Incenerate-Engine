#version 450

layout(binding = 0) uniform sampler2D color_tex;

layout(location = 0) in vec2 fragOffset;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outMRAH;
layout(location = 3) out vec4 outEmissive;
#ifdef EDITOR_MODE
layout(location = 4) out vec4 outObjectID;
#endif

layout(push_constant) uniform UBOBillboard
{
	mat4 projection;
	mat4 view;
	vec3 billboardPosition;
	vec3 color;
	vec4 object_id;
} billboard;

void main()
{
	vec4 albedo_map = texture(color_tex, inUV);
	if(albedo_map.a <= 0.5)
		discard;

	albedo_map.rgb *= billboard.color;

	outAlbedo = albedo_map;
	outNormal = vec4(0.0);
	outMRAH = vec4(0.0);
	outEmissive = vec4(0.0);
#ifdef EDITOR_MODE
	outObjectID = billboard.object_id;
#endif
}