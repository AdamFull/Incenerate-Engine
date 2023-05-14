#version 450
#extension GL_GOOGLE_include_directive : require

#define VERTICES_COUNT 4
 
layout (vertices = VERTICES_COUNT) out;

layout (location = 0) in vec2 inUV[];
layout (location = 1) in vec3 inColor[];
layout (location = 2) in vec3 inNormal[];

layout (location = 0) out vec2 outUV[VERTICES_COUNT];
layout (location = 1) out vec3 outColor[VERTICES_COUNT];
layout (location = 2) out vec3 outNormal[VERTICES_COUNT];

#include "shader_inputs.glsl"

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
	const float radius = 64.0f;
	vec4 pos = gl_in[gl_InvocationID].gl_Position;

#ifdef HAS_HEIGHTMAP
	pos.y += sample_texture(height_tex, inUV[0]).r * material.displacementStrength;
#endif

	// Check sphere against frustum planes
	for (int i = 0; i < 6; i++) 
	{
		if (dot(pos, data.frustumPlanes[i]) + radius < 0.0)
			return false;
	}
	return true;
}

void main()
{
	if (gl_InvocationID == 0)
    {
		mat4 mvp = data.projection * data.view;
        vec4 p0 = mvp * gl_in[0].gl_Position;
        vec4 p1 = mvp * gl_in[1].gl_Position;
        vec4 p2 = mvp * gl_in[2].gl_Position;
        vec4 p3 = mvp * gl_in[3].gl_Position;
        p0 /= p0.w;
        p1 /= p1.w;
        p2 /= p2.w;
        p3 /= p3.w;
        if (p0.z <= 0.0 || p1.z <= 0.0 || p2.z <= 0.0 || p3.z <= 0.0)
        {
             gl_TessLevelOuter[0] = 0.0;
             gl_TessLevelOuter[1] = 0.0;
             gl_TessLevelOuter[2] = 0.0;
             gl_TessLevelOuter[3] = 0.0;
        }
        else
        {
            //float l0 = length(p2.xy - p0.xy) * 16.0 + 1.0;
            //float l1 = length(p3.xy - p2.xy) * 16.0 + 1.0;
            //float l2 = length(p3.xy - p1.xy) * 16.0 + 1.0;
            //float l3 = length(p1.xy - p0.xy) * 16.0 + 1.0;
            //gl_TessLevelOuter[0] = l0;
            //gl_TessLevelOuter[1] = l1;
            //gl_TessLevelOuter[2] = l2;
            //gl_TessLevelOuter[3] = l3;
            //gl_TessLevelInner[0] = min(l1, l3);
            //gl_TessLevelInner[1] = min(l0, l2);
			gl_TessLevelOuter[0] = 16.0;
			gl_TessLevelOuter[1] = 16.0;
			gl_TessLevelOuter[2] = 16.0;
			gl_TessLevelOuter[3] = 16.0;
			gl_TessLevelInner[0] = 16.0;
			gl_TessLevelInner[1] = 16.0;
        }
    }
	
	gl_out[gl_InvocationID].gl_Position =  gl_in[gl_InvocationID].gl_Position;
	outUV[gl_InvocationID] = inUV[gl_InvocationID];
	outColor[gl_InvocationID] = inColor[gl_InvocationID];
	outNormal[gl_InvocationID] = inNormal[gl_InvocationID];
} 