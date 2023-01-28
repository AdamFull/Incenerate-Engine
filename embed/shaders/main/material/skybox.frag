#version 450
#extension GL_GOOGLE_include_directive : require

layout(binding = 1) uniform samplerCube samplerCubeMap;

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec4 inObjectID;

layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outMRAH;
layout(location = 3) out vec4 outEmissive;
layout(location = 4) out vec4 outObjectID;


#include "../../shader_util.glsl"

void main() 
{
	//vec3 albedo_map = pow(texture(samplerCubeMap, inWorldPos).rgb, vec3(1.2));
	//albedo_map = Uncharted2Tonemap(albedo_map);

	outAlbedo = texture(samplerCubeMap, inWorldPos);
	outNormal = vec4(0.0);
	outMRAH = vec4(0.0);
	outEmissive = vec4(0.0);
	outObjectID = inObjectID;
}