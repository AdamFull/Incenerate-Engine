#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inColor;

#include "shader_inputs.glsl"

void main() 
{
	vec2 texCoord = inUV;

	vec4 albedo_map = vec4(0.0);
#ifdef HAS_BASECOLORMAP
//BASECOLOR
	albedo_map = sample_texture(color_tex, texCoord) * material.baseColorFactor;
#else // HAS_BASECOLORMAP
	albedo_map = material.baseColorFactor;
#endif // HAS_BASECOLORMAP
	albedo_map *= vec4(inColor, 1.0);

	if(albedo_map.a <= material.alphaCutoff)
		discard;
}