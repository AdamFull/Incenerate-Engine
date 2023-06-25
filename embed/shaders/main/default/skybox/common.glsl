#ifndef DYNAMIC_SKYBOX_COMMON_GLSL
#define DYNAMIC_SKYBOX_COMMON_GLSL

#include "../../../math.glsl"
#include "constants.glsl"

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
	const vec3 c = vec3(.256098, .132268, .010016);
	const vec3 d = vec3(-1.5, -1.74, -1.98);
	const vec3 e = vec3(1.5625, 1.7569, 1.9801);
	return dot((x * x + 1.) * c / pow(d * x + e, vec3(1.5)), vec3(.33333333333));
}


vec3 intersectSphere(in vec3 origin, in vec3 direction, in float innerRadius, in float outerRadius)
{
    vec2 intersection = raySphereIntersect(origin, direction, vec3(0.0), outerRadius);
    float outerHit = intersection.x < 0 ? intersection.y : intersection.x;

    if (outerHit < 0.0)
        return origin;

    float maxDistance = (outerRadius - innerRadius) * 10;
    float shift = min(maxDistance, outerHit);

    return origin + direction * shift;
}

vec3 skyLighting(vec3 origin, vec3 direction, vec3 sunDirection, vec3 sunColor, float sunIntensity)
{
    const vec3 hitPos = intersectSphere(origin, direction, earthRadius, atmosphereRadius);

    if (length(hitPos - origin) < 0.01)
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

#if defined(SUN)
    const float theta = dot(direction, -sunDirection);
    const float zeta = cos(SunAngularR);
    if (theta < zeta)
    {
        return vec3(0);
    }
#endif

    for (uint i = 0; i < integralSteps - 1; i++)
    {
        const vec3 point = origin + rayStep * (i + 1);
        const float h = length(point) - earthRadius;

        const float hr = exp(-h / RayleighScaleHeight) * dStep;
        const float hm = exp(-h / MieScaleHeight) * dStep;
        const float ho = exp(-h / OzonScaleHeight) * dStep;

        densityR += hr;
        densityMie += hm;
        densityOzon += ho;

        const vec3  toLight = intersectSphere(point, -sunDirection, earthRadius, atmosphereRadius);
        const float hLight = length(toLight) - earthRadius;
        const float stepToLight = (hLight - h) / float(secondaryIntegralSteps);

        float dStepLight = length(toLight - point) / float(secondaryIntegralSteps);
        float densityLightR = 0.0f;
        float densityLightMie = 0.0f;
        float densityLightOzon = 0.0f;

        bool bReached = true;
        for (int j = 0; j < secondaryIntegralSteps; j++)
        {
            const float h1 = h + stepToLight * j;

            if (h1 < 0)
            {
                bReached = false;
                break;
            }

            densityLightMie += exp(-h1 / MieScaleHeight) * dStepLight;
            densityLightR += exp(-h1 / RayleighScaleHeight) * dStepLight;
            densityLightOzon += exp(-h1 / OzonScaleHeight) * dStepLight;
        }

        if (bReached)
        {
            vec3 aggr = exp(-BetaRayleigh * (densityR + densityLightR) - BetaMie * (densityLightMie + densityMie) - BetaOzon * (densityLightOzon + densityOzon));
            resR += aggr * hr;
            resMie += aggr * hm;
            resOzon += aggr * ho;
        }
    }

#if defined(SUN)
    // Sun disk
    vec2 intersection = raySphereIntersect(origin, direction, vec3(0), earthRadius);
    if (max(intersection.x, intersection.y) < 0.0f)
    {
        const float t = (1 - pow((1 - theta) / (1 - zeta), 2));
        const float attenuation = mix(0.83, 1.0f, t);
        const vec3 SunIlluminance = attenuation * vec3(1.0f) * 12000000.0f;
        const vec3 final = SunIlluminance;// * (resR * B0R * PhaseR(Angle) + B0Mie * resMie * PhaseMie(Angle));
        return final;
    }
    else
    {
        return vec3(0);
    }
#else
    return sunIntensity * (BetaRayleigh * resR * phaseReileight(angle) + BetaMie * resMie * phaseMie(angle) * sunColor + BetaOzon * resOzon * phaseOzon(angle) * vec3(0.5, 0.7, 1.0));
#endif
}

#endif