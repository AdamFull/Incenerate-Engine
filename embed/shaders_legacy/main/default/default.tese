#version 450
#extension GL_GOOGLE_include_directive : require

#include "shader_inputs.glsl"

layout(triangles, equal_spacing , cw) in;

layout (location = 0) in vec2 inUV[];
layout (location = 1) in vec3 inColor[];
layout (location = 2) in vec4 inPosition[];
#ifdef HAS_NORMALS
layout (location = 3) in vec3 inNormal[];
#endif
#ifdef HAS_TANGENTS
layout (location = 4) in vec4 inTangent[];
#endif
 
layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec4 outPosition;
#ifdef HAS_NORMALS
layout (location = 3) out vec3 outNormal;
#endif
#ifdef HAS_TANGENTS
layout (location = 4) out vec4 outTangent;
#endif

void main()
{
	outPosition = gl_TessCoord.x * inPosition[0] + gl_TessCoord.y * inPosition[1] + gl_TessCoord.z * inPosition[2];
	//gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) + (gl_TessCoord.y * gl_in[1].gl_Position) + (gl_TessCoord.z * gl_in[2].gl_Position); 

	outUV = gl_TessCoord.x * inUV[0] + gl_TessCoord.y * inUV[1] + gl_TessCoord.z * inUV[2];
    outColor = gl_TessCoord.x * inColor[0] + gl_TessCoord.y * inColor[1] + gl_TessCoord.z * inColor[2];

#ifdef HAS_NORMALS
	outNormal = gl_TessCoord.x * inNormal[0] + gl_TessCoord.y * inNormal[1] + gl_TessCoord.z * inNormal[2]; 
#endif

#ifdef HAS_TANGENTS
    outTangent = gl_TessCoord.x * inTangent[0] + gl_TessCoord.y * inTangent[1] + gl_TessCoord.z * inTangent[2]; 
#endif

#ifdef HAS_HEIGHTMAP
	vec3 displace = normalize(outNormal) * (max(texture(height_tex, outUV.st).r - 0.5f, 0.0) * material.displacementStrength);
    outPosition += vec4(displace, 0.0);
#endif

	outPosition = meshData.model * outPosition;
		
	gl_Position = data.projection * data.view * outPosition;
}