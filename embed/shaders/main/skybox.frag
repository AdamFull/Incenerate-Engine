#version 450
layout (binding = 1) uniform samplerCube samplerCubeMap;

layout (location = 0) in vec3 inWorldPos;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outPosition;
layout (location = 2) out vec4 outMask;
layout (location = 3) out vec4 outNormal;
layout (location = 4) out vec4 outAlbedo;
layout (location = 5) out vec4 outEmission;
layout (location = 6) out vec4 outMRAH;

void main() 
{
	vec3 color = texture(samplerCubeMap, inWorldPos).rgb;

	outPosition = vec4(inWorldPos, 1.0);
	outMask = vec4(0.0);
	outNormal = vec4(vec3(0.0), 1.0);
	outAlbedo = vec4(color, 1.0);
	//outMRAH = vec4(1.0);
	outColor = vec4(0.0);
}