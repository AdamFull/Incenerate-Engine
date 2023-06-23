#version 450
#extension GL_GOOGLE_include_directive : require

#include "../../../shader_util.glsl"
#include "../../../math.glsl"
#include "../../lightning_base.glsl"

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outMRAH;
layout(location = 3) out vec4 outEmissive;
#ifdef EDITOR_MODE
layout(location = 4) out vec4 outObjectID;
#endif

#ifndef MAX_DIRECTIONAL_LIGHTS_COUNT
#define MAX_DIRECTIONAL_LIGHTS_COUNT 1
#endif

//#define SUN

layout(std140, set = 0, binding = 0) uniform FCameraData 
{
  	mat4 view;
  	mat4 invView;
  	mat4 projection;
  	mat4 invProjection;
    mat4 invViewProjection;
} camera;

layout(std140, set = 0, binding = 1) uniform FAtmosphereScattering 
{
    int lightCount;
} scattering;

layout(std430, set = 0, binding = 2) buffer readonly UBOLights
{
	FDirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS_COUNT];
};

const float earthRadius = 6371e3;
const float atmosphereRadius = 6471e3; 

const int integralSteps = 128;
const int secondaryIntegralSteps = 8;

const float RayleighScaleHeight = 8000.0;
const float MieScaleHeight = 1200.0;
const vec3 BetaRayleigh = vec3(5.5e-6, 13.0e-6, 22.4e-6);
const vec3 BetaMie = vec3(2.0e-5);
const float OzonScaleHeight = 8.0f;
const float BetaOzon = 0.0025f;

vec2 raySphereIntersect(vec3 r0, vec3 rd, vec3 s0, float sr) {
    // - r0: ray origin
    // - rd: normalized ray direction
    // - s0: sphere center
    // - sr: sphere radius
    // - Returns distance from r0 to first intersecion with sphere,
    //   or -1.0 if no intersection.
    float a = dot(rd, rd);
    vec3 s0_r0 = r0 - s0;
    float b = 2.0 * dot(rd, s0_r0);
    float c = dot(s0_r0, s0_r0) - (sr * sr);
    if (b*b - 4.0*a*c < 0.0) {
        return vec2(-1.0, -1.0);
    }
    
    float t1 = (-b - sqrt((b*b) - 4.0*a*c))/(2.0*a);
    float t2 = (-b + sqrt((b*b) - 4.0*a*c))/(2.0*a);
    return vec2(t1, t2);
}

vec3 intersectSphere(in vec3 origin, in vec3 direction, in float innerRadius, in float outerRadius)
{
    vec2 intersection = raySphereIntersect(origin, direction, vec3(0.0), outerRadius);
    float outerHit = intersection.x < 0 ? intersection.y : intersection.x;

    if(outerHit < 0.0)
        return origin;

    float maxDistance = (outerRadius - innerRadius) * 10;
    float shift = min(maxDistance, outerHit);

    return origin + direction * shift;
}

float phaseReileight(float cosAngle)
{
  return ((3.0f * PI) / 16.0f) * (1.0f + cosAngle * cosAngle);
}

float phaseOzon(float cosAngle)
{
  return ((2.0f * PI) / 16.0f) * (1.0f + cosAngle * cosAngle);
}
  
  // The best variant with the precomputed values
  // taken from Github
  float phaseMie(float x)
  {
    const vec3 c = vec3(.256098,.132268,.010016);
    const vec3 d = vec3(-1.5,-1.74,-1.98);
    const vec3 e = vec3(1.5625,1.7569,1.9801);
    return dot((x * x + 1.) * c / pow( d * x + e, vec3(1.5)),vec3(.33333333333));
  }

vec3 skyLighting(vec3 origin, vec3 direction, vec3 sunDirection, vec3 sunColor, float sunIntensity)
{
    const vec3 hitPos = intersectSphere(origin, direction, earthRadius, atmosphereRadius);
     
     if(length(hitPos - origin) < 0.01)
     {
         return vec3(0);
     }

     const float angle = dot(normalize(hitPos - origin), -sunDirection);
           
     const vec3 rayStep = (hitPos - origin) / float(integralSteps);

     const float heightOrigin = length(origin);
     const float dh = (length(hitPos) - heightOrigin) / float(integralSteps);
     const float dStep = length(rayStep);
     
     vec3 resR = vec3(0.0f);
     vec3 resMie = vec3(0.0f);
     vec3 resOzon = vec3(0.0f);

     float densityR = 0.0f;
     float densityMie = 0.0f;
     float densityOzon = 0.0f;
     
     for(uint i = 0; i < integralSteps - 1; i++)
     {
         const vec3 point = origin + rayStep * (i + 1);
         const float h = length(point) - earthRadius;
         
         const float hr = exp(-h/RayleighScaleHeight)  * dStep;
         const float hm = exp(-h/MieScaleHeight) * dStep;
         const float ho = exp(-h / OzonScaleHeight) * dStep;
         
         densityR  += hr;
         densityMie += hm;
         densityOzon += ho;

         const vec3  toLight = intersectSphere(point, -sunDirection, earthRadius, atmosphereRadius);
         const float hLight  = length(toLight) - earthRadius;
         const float stepToLight = (hLight - h) / float(secondaryIntegralSteps);
         
         float dStepLight = length(toLight - point) / float(secondaryIntegralSteps);
         float densityLightR = 0.0f;
         float densityLightMie = 0.0f;
         float densityLightOzon = 0.0f;

         bool bReached = true;
         for(int j = 0; j < secondaryIntegralSteps; j++)
         {
            const float h1 = h + stepToLight * j;
            
            if(h1 < 0)
            {
                bReached = false;
                break;
            }
            
            densityLightMie += exp(-h1/MieScaleHeight) * dStepLight;
            densityLightR  += exp(-h1/RayleighScaleHeight)  * dStepLight;
            densityLightOzon += exp(-h1/OzonScaleHeight) * dStepLight;
         }
        
        if(bReached)
        {
            vec3 aggr = exp(-BetaRayleigh * (densityR + densityLightR) - BetaMie * (densityLightMie + densityMie) - BetaOzon * (densityLightOzon + densityOzon));
            resR  += aggr * hr;
            resMie += aggr * hm;
            resOzon += aggr * ho;
        }
    }
    
    return sunIntensity * (BetaRayleigh * resR * phaseReileight(angle) + BetaMie * resMie * phaseMie(angle) * sunColor + BetaOzon * resOzon * phaseOzon(angle) * vec3(0.5, 0.7, 1.0));
}

void main() 
{
    vec4 dirWorldSpace = vec4(0);
    const vec3 viewerPosition = vec3(camera.invView[3]);
    const vec3 origin = vec3(0, earthRadius, 0) + viewerPosition * 0.01;

    dirWorldSpace.xyz = getPositionFromDepth(vec2(inUV.x, 1.0 - inUV.y), 1.0f, camera.invViewProjection).xyz;

    vec3 finalColor = vec3(0.0);
    for(int i = 0; i < scattering.lightCount; ++i)
    {
        const FDirectionalLight light = directionalLights[i];
        const vec3 dirToSun = normalize(-light.direction);
    
        finalColor += skyLighting(origin, dirWorldSpace.xyz, -dirToSun, light.color, light.intencity);
    }
    
    finalColor /= float(scattering.lightCount);
    finalColor = 1.0 - exp(-1.0 * finalColor);

	outAlbedo = vec4(finalColor, 1.0);
	//outAlbedo = texture(samplerCubeMap, inWorldPos);
	outNormal = vec4(0.0);
	outMRAH = vec4(0.0);
	outEmissive = vec4(0.0);
#ifdef EDITOR_MODE
	//outObjectID = ubo.object_id;
#endif
}