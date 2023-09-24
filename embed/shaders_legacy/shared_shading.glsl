#include "math.glsl"

#ifndef SHARED_FUNC
#define SHARED_FUNC

vec3 cubeDir(vec2 texCoord, uint side) 
{
	vec2 tex = texCoord * 2.0f - 1.0f;
	if(side == 0) return vec3(1.0f, -tex.y, -tex.x); // Front
	if(side == 1) return vec3(-1.0f, -tex.y, tex.x); // Back
	if(side == 2) return vec3(tex.x, 1.0f, tex.y); // Right
	if(side == 3) return vec3(tex.x, -1.0f, -tex.y); // Left
	if(side == 4) return vec3(tex.x, -tex.y, 1.0f); // Top
	if(side == 5) return vec3(-tex.x, -tex.y, -1.0f); // Bottom
	return vec3(1.0);
}

// Normal Distribution function --------------------------------------
float D_GGX(float dotNH, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = dotNH * dotNH * (alpha2 - 1.0f) + 1.0f;
	return (alpha2)/(PI * denom*denom); 
}

// Geometric Shadowing function --------------------------------------
float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
	float r = (roughness + 1.0f);
	float k = (r*r) / 8.0f;
	float GL = dotNL / (dotNL * (1.0f - k) + k);
	float GV = dotNV / (dotNV * (1.0f - k) + k);
	return GL * GV;
}

// Fresnel function ----------------------------------------------------
vec3 F_Schlick(float cosTheta, float f90, vec3 F0)
{
	return F0 + (f90 - F0) * pow(1.0f - cosTheta, 5.0f);
}

vec3 F_SchlickR(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - cosTheta, 5.0f);
}

// Based omn http://byteblacksmith.com/improvements-to-the-canonical-one-liner-glsl-rand-for-opengl-es-2-0/
float random(vec2 co)
{
	float a = 12.9898f;
	float b = 78.233f;
	float c = 43758.5453f;
	float dt= dot(co.xy ,vec2(a,b));
	float sn= mod(dt,3.14f);
	return fract(sin(sn) * c);
}

vec2 hammersley2d(uint i, uint N) 
{
	// Radical inverse based on http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
	uint bits = (i << 16u) | (i >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	float rdi = float(bits) * 2.3283064365386963e-10;
	return vec2(float(i) /float(N), rdi);
}

// Based on http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_slides.pdf
vec3 importanceSample_GGX(vec2 Xi, float roughness, vec3 normal) 
{
	// Maps a 2D point to a hemisphere with spread based on roughness
	float alpha = roughness * roughness;
	float phi = 2.0f * PI * Xi.x + random(normal.xz) * 0.1f;
	float cosTheta = sqrt((1.0f - Xi.y) / (1.0f + (alpha*alpha - 1.0f) * Xi.y));
	float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
	vec3 H = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);

	// Tangent space
	vec3 up = abs(normal.z) < 0.999 ? vec3(0.0f, 0.0f, 1.0f) : vec3(1.0f, 0.0f, 0.0f);
	vec3 tangentX = normalize(cross(up, normal));
	vec3 tangentY = normalize(cross(normal, tangentX));

	// Convert to world Space
	return normalize(tangentX * H.x + tangentY * H.y + normal * H.z);
}

vec3 prefilteredReflection(vec3 R, float roughness, samplerCube prefiltered) 
{
	float lod = roughness * float(textureQueryLevels(prefiltered));
	float lodf = floor(lod);
	float lodc = ceil(lod);
	vec3 a = pow(textureLod(prefiltered, R, lodf).rgb, vec3(2.2f));
	vec3 b = pow(textureLod(prefiltered, R, lodc).rgb, vec3(2.2f));
	return mix(a, b, lod - lodf);
}


// microfacet model developed by Torrance-Sparrow
vec3 evaluateCookTorrenceSpecularBRDF(float D, float G, vec3 F, float cosThetaI, float cosThetaO) {

    return vec3((D * G * F) / (4.f * cosThetaI * cosThetaO));

}

#endif