#version 450

layout(location = 0) in vec2 inUV[];
layout(location = 1) in vec3 inColor[];
layout(location = 2) in vec3 inNormal[];

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec3 outColor;
layout(location = 2) out vec4 outPosition;
layout(location = 3) out vec3 outNormal;

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

#include "shader_inputs.glsl"

void main() 
{
 for (int i = 0; i < gl_in.length(); i++)
 {
     vec4 v0 = gl_in[i].gl_Position;
       vec2 t0 = inUV[i];
#ifdef HAS_HEIGHTMAP
       float h0 = sample_texture(height_tex, t0).r * material.displacementStrength;
#else
       float h0 = 0.0;
#endif
       v0.y += h0;
       
       vec4 v1 = gl_in[(i + 1) % 3].gl_Position;
       vec2 t1 = inUV[(i + 1) % 3];
#ifdef HAS_HEIGHTMAP
       float h1 = sample_texture(height_tex, t1).r * material.displacementStrength;
#else
       float h1 = 0.0;
#endif
       v1.y += h1;
       
       vec4 v2 = gl_in[(i + 2) % 3].gl_Position;
       vec2 t2 = inUV[(i + 2) % 3];
#ifdef HAS_HEIGHTMAP
       float h2 = sample_texture(height_tex, t2).r * material.displacementStrength;
#else
       float h2 = 0.0;
#endif
       v2.y += h2;
       
       vec3 edge1 = v1.xyz - v0.xyz;
       vec3 edge2 = v2.xyz - v0.xyz;
       vec3 normal = normalize(cross(edge1, edge2));
       
       outNormal = normalize(data.normal * vec4(normal, 1.0)).xyz;;
       outPosition = v0;
       outUV = t0;
       outColor = inColor[i];

       gl_Position = data.projection * data.view * outPosition;
       EmitVertex();
 }
 
   EndPrimitive();
}