#version 450

layout(location = 0) in vec2 fragOffset;

layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outMRAH;
layout(location = 3) out vec4 outEmissive;
#ifdef EDITOR_MODE
layout(location = 4) out vec4 outObjectID;
#endif

void main()
{
	outAlbedo = vec4(1.0);
	outNormal = vec4(0.0);
	outMRAH = vec4(0.0);
	outEmissive = vec4(0.0);
#ifdef EDITOR_MODE
	outObjectID = vec(0.0);
#endif
}