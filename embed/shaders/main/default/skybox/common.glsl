#ifndef DYNAMIC_SKYBOX_COMMON_GLSL
#define DYNAMIC_SKYBOX_COMMON_GLSL

#include "../../../math.glsl"
#include "constants.glsl"


float phaseRayleight(float cosAngle)
{
    const float cosAngleSquare = cosAngle * cosAngle;
    return ((3.0 * PI) / 16.0) * (1.0 + cosAngleSquare);
}

float phaseMie(float cosAngle)
{
    const float cosAngleSquare = cosAngle * cosAngle;
    const float gSquare = g * g;
    return ((3.0 * PI) / 8.0) * (gSquare * (1.0 + cosAngleSquare)) / pow((2.0 + gSquare) * (1.0 + gSquare - 2.0 * g * cosAngle), 1.5);
}

// Calculate Rayleight, Mie and Absorbtion density 
vec3 calculateDensity(float height, float deltaHeight)
{
    const vec2 scaleHeight = vec2(RayleighScaleHeight, MieScaleHeight);

    vec3 density = vec3(exp(-height / scaleHeight), 0.0);

    float denom = (AbsorptionScaleHeight - height) / FalloffAbsorbtion;
    density.z = (1.0 / (denom * denom + 1.0)) * density.x;

    density *= deltaHeight;

    return density;
}

vec3 atmosphere(vec3 origin, vec3 direction, vec3 sunDirection, vec3 sunColor, float sunIntensity)
{
    // Trace ray from viewer position to atmosphere
    vec2 intersection = raySphereIntersect(origin, direction, vec3(0.0), atmosphereRadius);

    // Prepare his distances to calculations
    intersection.x = max(intersection.x, 0.0);
    intersection.y = min(intersection.y, maxScatterDistance);

    // Discard when first intersection distance is higher then second
    if (intersection.x > intersection.y)
        return defaultColor;

    // Calculate point where ray hit a sphere
    const float hitDistance = intersection.y - intersection.x;
    const vec3 hitPosition = direction * hitDistance;
    const vec3 rayStep = hitPosition / float(integralStepCount);
    const float stepHeight = length(rayStep);

    vec3 opticalDepth = vec3(0.0);
    vec3 totalRayleight = vec3(0.0);
    vec3 totalMie = vec3(0.0);

    // Solve first integral
    for (int i = 0; i < integralStepCount; ++i)
    {
        const vec3 integralPos = origin + rayStep * i;
        const float integralHeight = length(integralPos) - planetRadius;

        // Accumulate air density
        const vec3 density = calculateDensity(integralHeight, stepHeight);
        opticalDepth += density;

        // Trace ray from current step position to sun direction
        vec2 sunIntersection = raySphereIntersect(integralPos, -sunDirection, vec3(0.0), atmosphereRadius);

        // Calculate point where ray hit a sphere
        const vec3 sunHitPosition = -sunDirection * sunIntersection.y;
        const vec3 sunRayStep = sunHitPosition / float(lightIntegralStepCount);
        const float sunStepHeight = length(sunRayStep);

        vec3 sunOpticalDepth = vec3(0.0);

        // Solve second integral
        for (int j = 0; j < lightIntegralStepCount; ++j)
        {
            const vec3 sunIntegralPos = integralPos + sunRayStep * j;
            const float sunIntegralHeight = length(sunIntegralPos) - planetRadius;

            // Accumulate sun density
            const vec3 sunDensity = calculateDensity(sunIntegralHeight, sunStepHeight);
            sunOpticalDepth += sunDensity;
        }

        // Calculate attenuation
        vec3 attenuation = exp(-BetaRayleigh * (opticalDepth.x + sunOpticalDepth.x) - BetaMie * (opticalDepth.y + sunOpticalDepth.y) - BetaAbsorption * (opticalDepth.z + sunOpticalDepth.z));

        totalRayleight += density.x * attenuation;
        totalMie += density.y * attenuation;
    }

    vec3 opacity = exp(-(BetaRayleigh * opticalDepth.x + BetaMie * opticalDepth.y + BetaAbsorption * opticalDepth.z));

    // Angle between viewer direction and sun direction
    const float angle = dot(normalize(direction), -sunDirection);

    // Calculate total scattered color
    vec3 totalScatter = phaseRayleight(angle) * BetaRayleigh * totalRayleight + phaseMie(angle) * BetaMie * totalMie; // +normalize(opticalDepth.z * betaAmbient);

    return totalScatter * sunIntensity + defaultColor * opacity;
}

#endif