#version 450
#extension GL_ARB_shader_viewport_layer_array : require
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec4 inTangent;
layout(location = 5) in vec4 inWeight0;
layout(location = 6) in vec4 inJoint0;

//layout(location = 0) out vec2 outUV;
//layout(location = 1) out vec3 outColor;

#include "shader_inputs.glsl"

layout(push_constant) uniform UBOShadowmapModelData
{
	mat4 model;
	mat4 viewProjMat;
	int layer;
	int hasSkin;
} modelData;

void main()
{
	mat4 instanceModel = instancing.instances[gl_InstanceIndex].model;
	vec4 instanceColor = instancing.instances[gl_InstanceIndex].color;

	mat4 modelMatrix = instanceModel * modelData.model;
	//mat4 modelMatrix = modelData.model;

	vec4 localPos;
	if(modelData.hasSkin > 0)
	{
		mat4 skinMat = 
		inWeight0.x * jointMatrices[int(inJoint0.x)] +
		inWeight0.y * jointMatrices[int(inJoint0.y)] +
		inWeight0.z * jointMatrices[int(inJoint0.z)] +
		inWeight0.w * jointMatrices[int(inJoint0.w)];
		
		localPos = modelMatrix * skinMat * vec4(inPosition, 1.0);
	}
	else
	{
		localPos = modelMatrix * vec4(inPosition, 1.0);
	}

	localPos = vec4(localPos.xyz / localPos.w, 1.0);

	//outUV = inTexCoord;
	//outColor = inColor * instanceColor.rgb;

	gl_Layer = modelData.layer;
	gl_Position = modelData.viewProjMat * localPos;
}