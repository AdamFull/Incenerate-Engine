#version 460

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inLightPos;

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(inPosition.xyz - inLightPos.xyz);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / 32.f;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}  