#version 450
#extension GL_GOOGLE_include_directive : require

layout(binding = 1) uniform samplerCube samplerCubeMap;

layout(location = 0) in vec3 inWorldPos;

layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outMRAH;
layout(location = 3) out vec4 outEmissive;
#ifdef EDITOR_MODE
layout(location = 4) out vec4 outObjectID;
#endif


layout(binding = 0) uniform FUniformData 
{
	mat4 model;
  	mat4 view;
  	mat4 projection;
  	mat4 normal;
	vec3 viewDir;
	vec2 viewportDim;
	vec4 frustumPlanes[6];
	vec4 object_id;
} ubo;

#include "../../shader_util.glsl"

void main() 
{
	outAlbedo = texture(samplerCubeMap, inWorldPos);
	outNormal = vec4(0.0);
	outMRAH = vec4(0.0);
	outEmissive = vec4(0.0);
#ifdef EDITOR_MODE
	outObjectID = ubo.object_id;
#endif
}