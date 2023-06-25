#ifndef DYNAMIC_SKYBOX_CONSTANTS_GLSL
#define DYNAMIC_SKYBOX_CONSTANTS_GLSL

const float earthRadius = 6371e3;
const float atmosphereRadius = 6531e3;
const float cloudsStart = earthRadius + 7000.0f;
const float cloudsEnd = earthRadius + 15000.0f;

const float SunAngularR = radians(0.545f);

const int integralSteps = 128;
const int secondaryIntegralSteps = 8;

const float RayleighScaleHeight = 8000.0;
const float MieScaleHeight = 1200.0;
const vec3 BetaRayleigh = vec3(5.5e-6, 13.0e-6, 22.4e-6);
const vec3 BetaMie = vec3(2.0e-5);
const float OzonScaleHeight = 8.0f;
const float BetaOzon = 0.0025f;

#endif