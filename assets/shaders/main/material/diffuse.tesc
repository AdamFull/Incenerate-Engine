#version 450
#extension GL_GOOGLE_include_directive : require

layout(std140, binding = 7) uniform UBOMaterial
{
	vec4 baseColorFactor;
	vec3 emissiveFactor;
	float alphaCutoff;
	float normalScale;
	float occlusionStrenth;
	float metallicFactor;
	float roughnessFactor;
	float tessellationFactor;
	float tessellationStrength;
} material;

layout(std140, binding = 0) uniform FUniformData 
{
  	mat4 model;
	mat4 model_old;
  	mat4 view;
  	mat4 projection;
  	mat4 normal;
	vec3 viewDir;
	vec2 viewportDim;
	vec4 frustumPlanes[6];
} data;

#ifdef HAS_HEIGHTMAP
layout(binding = 6) uniform sampler2D height_tex;
#endif

#define VERTICES_COUNT 3
 
layout (vertices = VERTICES_COUNT) out;

layout (location = 0) in vec2 inUV[];
layout (location = 1) in vec3 inColor[];
layout (location = 2) in vec4 inPosition[];
layout (location = 3) in vec4 inOldPosition[];
#ifdef HAS_NORMALS
layout (location = 4) in vec3 inNormal[];
#endif
#ifdef HAS_TANGENTS
layout (location = 5) in vec4 inTangent[];
#endif

layout (location = 0) out vec2 outUV[VERTICES_COUNT];
layout (location = 1) out vec3 outColor[VERTICES_COUNT];
layout (location = 2) out vec4 outPosition[VERTICES_COUNT];
layout (location = 3) out vec4 outOldPosition[VERTICES_COUNT];
#ifdef HAS_NORMALS
layout (location = 4) out vec3 outNormal[VERTICES_COUNT];
#endif
#ifdef HAS_TANGENTS
layout (location = 5) out vec4 outTangent[VERTICES_COUNT];
#endif

// Calculate the tessellation factor based on screen space
// dimensions of the edge
float screenSpaceTessFactor(vec4 p0, vec4 p1)
{
	// Calculate edge mid point
	vec4 midPoint = 0.5 * (p0 + p1);
	// Sphere radius as distance between the control points
	float radius = distance(p0, p1) / 2.0;

	// View space
	vec4 v0 = data.view * data.model  * midPoint;

	// Project into clip space
	vec4 clip0 = (data.projection * (v0 - vec4(radius, vec3(0.0))));
	vec4 clip1 = (data.projection * (v0 + vec4(radius, vec3(0.0))));

	// Get normalized device coordinates
	clip0 /= clip0.w;
	clip1 /= clip1.w;

	// Convert to viewport coordinates
	clip0.xy *= data.viewportDim;
	clip1.xy *= data.viewportDim;
	
	// Return the tessellation factor based on the screen size 
	// given by the distance of the two edge control points in screen space
	// and a reference (min.) tessellation size for the edge set by the application
	return clamp(distance(clip0, clip1) / 10.0 * material.tessellationFactor, 1.0, 64.0);
}

// Checks the current's patch visibility against the frustum using a sphere check
// Sphere radius is given by the patch size
bool frustumCheck()
{
	// Fixed radius (increase if patch size is increased in example)
	const float radius = 20.0f;
	vec4 pos = gl_in[gl_InvocationID].gl_Position;
#ifdef HAS_HEIGHTMAP
	pos.y -= texture(height_tex, inUV[0]).r * material.tessellationStrength;
#endif

	// Check sphere against frustum planes
	for (int i = 0; i < 6; i++) {
		if (dot(pos, data.frustumPlanes[i]) + radius < 0.0)
		{
			return false;
		}
	}
	return true;
}

void main()
{
	if (gl_InvocationID == 0)
	{
		if (!frustumCheck())
		{
			gl_TessLevelInner[0] = 0.0;
			gl_TessLevelOuter[0] = 0.0;
			gl_TessLevelOuter[1] = 0.0;
			gl_TessLevelOuter[2] = 0.0;
		}
		else
		{
			if (material.tessellationFactor > 0.0)
			{
				gl_TessLevelOuter[0] = screenSpaceTessFactor(gl_in[1].gl_Position, gl_in[2].gl_Position);
				gl_TessLevelOuter[1] = screenSpaceTessFactor(gl_in[2].gl_Position, gl_in[0].gl_Position);
				gl_TessLevelOuter[2] = screenSpaceTessFactor(gl_in[0].gl_Position, gl_in[1].gl_Position);
				gl_TessLevelInner[0] = gl_TessLevelOuter[2];
			}
			else
			{
				// Tessellation factor can be set to zero by example
				// to demonstrate a simple passthrough
				gl_TessLevelInner[0] = 1.0;
				gl_TessLevelOuter[0] = 1.0;
				gl_TessLevelOuter[1] = 1.0;
				gl_TessLevelOuter[2] = 1.0;
			}
		}
	}

	//gl_out[gl_InvocationID].gl_Position =  gl_in[gl_InvocationID].gl_Position;
    outUV[gl_InvocationID] = inUV[gl_InvocationID];
    outColor[gl_InvocationID] = inColor[gl_InvocationID];
	outPosition[gl_InvocationID] = inPosition[gl_InvocationID];
	outOldPosition[gl_InvocationID] = inOldPosition[gl_InvocationID];
#ifdef HAS_NORMALS
	outNormal[gl_InvocationID] = inNormal[gl_InvocationID];
#endif
#ifdef HAS_TANGENTS
    outTangent[gl_InvocationID] = inTangent[gl_InvocationID];
#endif
} 