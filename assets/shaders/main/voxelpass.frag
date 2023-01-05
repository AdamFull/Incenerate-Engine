#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_ARB_texture_cube_map_array : enable
#extension GL_GOOGLE_include_directive : require

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragcolor;

void main() 
{
    outFragcolor = vec4(inUV, 1.f, 1.f);
}