#version 450
#extension GL_GOOGLE_include_directive : require

layout(std140, binding = 0) uniform FUniformData 
{
  	mat4 model;
  	mat4 view;
  	mat4 projection;
  	mat4 normal;
	vec3 viewDir;
	vec2 viewportDim;
	vec4 frustumPlanes[6];
	vec4 object_id;
} data;

layout(std140, binding = 19) uniform UBOMaterial
{
	vec4 baseColorFactor;
	vec3 emissiveFactor;
	float emissiveStrength;
	int alphaMode;
	float alphaCutoff;
	float normalScale;
	float occlusionStrenth;
	float metallicFactor;
	float roughnessFactor;
	float tessellationFactor;
	float tessellationStrength;
} material;

#ifdef HAS_HEIGHTMAP
layout(binding = 7) uniform sampler2D height_tex;
#endif

layout(quads, equal_spacing , cw) in;

layout (location = 0) in vec2 inUV[];
layout (location = 1) in vec3 inColor[];
layout (location = 2) in vec3 inNormal[];

 
layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec4 outPosition;
layout (location = 3) out vec3 outNormal;

void main()
{
	// Interpolate UV coordinates
	vec2 uv1 = mix(inUV[0], inUV[1], gl_TessCoord.x);
	vec2 uv2 = mix(inUV[3], inUV[2], gl_TessCoord.x);
	outUV = mix(uv1, uv2, gl_TessCoord.y);

	vec3 n1 = mix(inNormal[0], inNormal[1], gl_TessCoord.x);
	vec3 n2 = mix(inNormal[3], inNormal[2], gl_TessCoord.x);
	outNormal = mix(n1, n2, gl_TessCoord.y);

	// Interpolate positions
	vec4 pos1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 pos2 = mix(gl_in[3].gl_Position, gl_in[2].gl_Position, gl_TessCoord.x);
	outPosition = mix(pos1, pos2, gl_TessCoord.y);

	// Interpolate color
	vec3 color1 = mix(inColor[0], inColor[1], gl_TessCoord.x);
	vec3 color2 = mix(inColor[3], inColor[2], gl_TessCoord.x);
	outColor = mix(color1, color2, gl_TessCoord.y);

	// Displace
#ifdef HAS_HEIGHTMAP
	outPosition.y += textureLod(height_tex, outUV, 0.0).r * material.tessellationStrength;
#endif

	// Perspective projection
	gl_Position = data.projection * data.view * outPosition;
}