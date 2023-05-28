#version 460

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inLightPos;

layout (binding = 0) uniform UBOShadowmap 
{
	mat4 viewProjMat;
	vec4 position;
	int stride;
    float farPlane;
} uboshadow;

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(inPosition.xyz - inLightPos.xyz);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / uboshadow.farPlane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}  