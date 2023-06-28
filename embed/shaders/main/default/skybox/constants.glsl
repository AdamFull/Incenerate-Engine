#ifndef DYNAMIC_SKYBOX_CONSTANTS_GLSL
#define DYNAMIC_SKYBOX_CONSTANTS_GLSL

const float planetRadius = 6371e3;
const float atmosphereRadius = 6471e3;
const float maxScatterDistance = (atmosphereRadius - planetRadius) * 2.0;
const float cloudsStart = planetRadius + 7000.0f;
const float cloudsEnd = planetRadius + 15000.0f;

const vec3 betaAmbient = vec3(0.01);
const vec3 defaultColor = vec3(0.001);

const float SunAngularR = radians(0.545f);

const int integralStepCount = 32;
const int lightIntegralStepCount = 16;

const float g = 0.7;
const float RayleighScaleHeight = 8e3;
const vec3 BetaRayleigh = vec3(5.5e-6, 13.0e-6, 22.4e-6);

const float MieScaleHeight = 1.2e3;
const vec3 BetaMie = vec3(21e-6);

const float AbsorptionScaleHeight = 30e3;
const vec3 BetaAbsorption = vec3(2.04e-5, 4.97e-5, 1.95e-6);
const float FalloffAbsorbtion = 4e3;

#endif