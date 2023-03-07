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
	float displacementStrength;
} material;

layout(binding = 7) uniform sampler2D height_tex;

layout(quads, equal_spacing, cw) in;

layout (location = 0) in vec2 inUV[];
layout (location = 1) in vec3 inColor[];
layout (location = 2) in vec3 inNormal[];

 
layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec4 outPosition;
layout (location = 3) out vec3 outNormal;

vec2 interpolate(vec2 p0, vec2 p1, vec2 p2, vec2 p3)
{
	vec2 pp1 = mix(p0, p1, gl_TessCoord.x);
	vec2 pp2 = mix(p3, p2, gl_TessCoord.x);
	return mix(pp1, pp2, gl_TessCoord.y);
}

vec3 interpolate(vec3 p0, vec3 p1, vec3 p2, vec3 p3)
{
	vec3 pp1 = mix(p0, p1, gl_TessCoord.x);
	vec3 pp2 = mix(p3, p2, gl_TessCoord.x);
	return mix(pp1, pp2, gl_TessCoord.y);
}

vec4 interpolate(vec4 p0, vec4 p1, vec4 p2, vec4 p3)
{
	vec4 pp1 = mix(p0, p1, gl_TessCoord.x);
	vec4 pp2 = mix(p3, p2, gl_TessCoord.x);
	return mix(pp1, pp2, gl_TessCoord.y);
}

vec3 normal_from_height(sampler2D heightmap, in vec2 uv)
{
	const vec2 texelSize = vec2(1.0) / textureSize(heightmap, 0);
	float centerHeight = textureLod(heightmap, uv, 0).r;
    float leftHeight = textureLod(heightmap, uv - vec2(texelSize.x, 0), 0).r;
    float rightHeight = textureLod(heightmap, uv + vec2(texelSize.x, 0), 0).r;
    float bottomHeight = textureLod(heightmap, uv - vec2(0, texelSize.y), 0).r;
    float topHeight = textureLod(heightmap, uv + vec2(0, texelSize.y), 0).r;
	
    vec3 dx = vec3(texelSize.x * 2.0, rightHeight - leftHeight, 0.0);
    vec3 dy = vec3(0.0, topHeight - bottomHeight, texelSize.y * 2.0);
	
    return normalize(cross(dy, dx));
}

void main()
{
	// Interpolate UV coordinates
    outUV = interpolate(inUV[0], inUV[1], inUV[2], inUV[3]);

	float height = textureLod(height_tex, outUV, 0).r;

    outNormal = normal_from_height(height_tex, outUV);
	outNormal = normalize(data.normal * vec4(outNormal, 1.0)).xyz;

    // Interpolate positions with smoothing
    outPosition = interpolate(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);
	outPosition.y += height * material.displacementStrength;

    // Interpolate color
    outColor = interpolate(inColor[0], inColor[1], inColor[2], inColor[3]);

    // Perspective projection
    gl_Position = data.projection * data.view * outPosition;
}