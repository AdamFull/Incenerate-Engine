#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

//Based on https://github.com/RoundedGlint585/ScreenSpaceReflection/blob/master/shaders/SSRFragment.glsl

#include "../shared_shading.glsl"

layout (set = 1, binding = 0) uniform sampler2D depth_tex;
layout (set = 1, binding = 1) uniform sampler2D mrah_tex;
layout (set = 1, binding = 2) uniform sampler2D normal_tex;
layout (set = 1, binding = 3) uniform sampler2D albedo_tex;

//--------------------In/Out locations--------------------
layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragcolor;

//--------------------Uniforms--------------------
layout (std140, set = 0, binding = 0) uniform UBOGeneralMatrices
{
    mat4 projection;
    mat4 invProjection;
	mat4 view;
	mat4 invView;
} data;

layout (std140, set = 0, binding = 1) uniform UBOReflections
{
    float rayStep;
	int iterationCount;
	float distanceBias;
	int sampleCount;
	float samplingCoefficient;
	float reflectionSpecularFalloffExponent;
	int debugDraw;
	int isBinarySearchEnabled;
	int isAdaptiveStepEnabled;
	int isExponentialStepEnabled;
	int isSamplingEnabled;
} refl;

vec3 generatePositionFromDepth(vec2 texturePos, float depth) 
{
	vec4 ndc = vec4((texturePos - 0.5) * 2, depth, 1.f);
	vec4 inversed = data.invProjection * ndc;
	inversed /= inversed.w;
	return inversed.xyz;
}

vec2 generateProjectedPosition(vec3 pos)
{
	vec4 samplePosition = data.projection * vec4(pos, 1.f);
	samplePosition.xy = (samplePosition.xy / samplePosition.w) * 0.5 + 0.5;
	return samplePosition.xy;
}

vec2 SSR(vec3 position, vec3 reflection, out vec3 color) 
{
	vec3 stp = refl.rayStep * reflection;
	vec3 marchingPosition = position + stp;
	float delta;
	float depthFromScreen;
	vec2 screenPosition;
	
	int i = 0;
	for (; i < refl.iterationCount; i++) 
	{
		screenPosition = generateProjectedPosition(marchingPosition);
		depthFromScreen = abs(generatePositionFromDepth(screenPosition, texture(depth_tex, screenPosition).x).z);
		delta = abs(marchingPosition.z) - depthFromScreen;

		if (abs(delta) < refl.distanceBias) 
		{
			color = vec3(1);
			if(refl.debugDraw > 0)
				color = vec3( 0.5+ sign(delta)/2,0.3,0.5- sign(delta)/2);
			return screenPosition;
		}

		if (refl.isBinarySearchEnabled > 0 && delta >= 0)
			break;

		if (refl.isAdaptiveStepEnabled > 0)
		{
			float directionSign = sign(abs(marchingPosition.z) - depthFromScreen);
			//this is sort of adapting step, should prevent lining reflection by doing sort of iterative converging
			//some implementation doing it by binary search, but I found this idea more cheaty and way easier to implement
			stp = stp * (1.0 - refl.rayStep * max(directionSign, 0.0));
			marchingPosition += stp * (-directionSign);
		}
		else
			marchingPosition += stp;
	
		if (refl.isExponentialStepEnabled > 0)
			stp *= 1.05;
    }


	if(refl.isBinarySearchEnabled > 0)
	{
		for(; i < refl.iterationCount; i++){
			
			stp *= 0.5;
			marchingPosition = marchingPosition - stp * sign(delta);
			
			screenPosition = generateProjectedPosition(marchingPosition);
			depthFromScreen = abs(generatePositionFromDepth(screenPosition, texture(depth_tex, screenPosition).x).z);
			delta = abs(marchingPosition.z) - depthFromScreen;
			
			if (abs(delta) < refl.distanceBias) 
			{
                color = vec3(1);
                if(refl.debugDraw > 0)
                    color = vec3( 0.5+ sign(delta)/2,0.3,0.5- sign(delta)/2);
				return screenPosition;
			}
		}
	}
	
    return vec2(0.0);
}

void main()
{
	vec3 position = generatePositionFromDepth(inUV, texture(depth_tex, inUV).x);
	vec4 normal = data.view * vec4(texture(normal_tex, inUV).xyz, 0.0);
	vec4 mrah = texture(mrah_tex, inUV);

	float metallic = mrah.g;
	float roughness = mrah.r;

	vec3 albedo = texture(albedo_tex, inUV).xyz;
	//albedo = mix(albedo, vec3(0.04), metallic);

	vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
	vec3 fresnel = F_SchlickR(max(dot(normalize(normal.rgb), normalize(position)), 0.0), F0, roughness);

	if(metallic < 0.01)
		outFragcolor = vec4(albedo, 1.0);
	else
	{
		vec3 reflectionDirection = normalize(reflect(position, normalize(normal.xyz)));
		if (refl.isSamplingEnabled > 0)
		{
			vec3 firstBasis = normalize(cross(vec3(0.f, 0.f, 1.f), reflectionDirection));
			vec3 secondBasis = normalize(cross(reflectionDirection, firstBasis));
			vec4 resultingColor = vec4(0.f);

			for (int i = 0; i < refl.sampleCount; i++) 
			{
				vec2 coeffs = vec2(random(inUV + vec2(0, i)) + random(inUV + vec2(i, 0))) * refl.samplingCoefficient * roughness;
				vec3 reflectionDirectionRandomized = reflectionDirection + firstBasis * coeffs.x + secondBasis * coeffs.y;

				vec3 debugColor;
				vec2 coords = SSR(position, normalize(reflectionDirectionRandomized), debugColor);
				if (coords != vec2(0.0f))
				{
					vec4 reflColor = texture(albedo_tex, coords);
					reflColor.rgb *= debugColor;

					vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));
					float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
					float reflectionMultiplier = pow(metallic, refl.reflectionSpecularFalloffExponent) * screenEdgefactor * -reflectionDirection.z;

					reflColor.rgb *= clamp(reflectionMultiplier, 0.0, 0.9) * fresnel;

					resultingColor += reflColor;
				}
			}

			if (resultingColor.w == 0)
				outFragcolor = texture(albedo_tex, inUV);
			else 
			{
				resultingColor /= resultingColor.w;
				//outFragcolor = vec4(mix(albedo, resultingColor.xyz, metallic), 1.f);
				outFragcolor = vec4(resultingColor.rgb, 1.f);
			}
		}
		else
		{
			// Applying roughness to resulting reflection vertor
			vec2 coeffs = vec2(random(inUV)) * roughness;
			vec3 firstBasis = normalize(cross(vec3(0.f, 0.f, 1.f), reflectionDirection));
			vec3 secondBasis = normalize(cross(reflectionDirection, firstBasis));
			reflectionDirection += firstBasis * coeffs.x + secondBasis * coeffs.y;

			vec3 debugColor;
			vec2 coords = SSR(position, normalize(reflectionDirection), debugColor);
			if (coords == vec2(0.f))
				outFragcolor = vec4(albedo, 1.0);
			else
			{
				vec4 reflColor = texture(albedo_tex, coords);
				reflColor.rgb *= debugColor;

				vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));
				float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
				float reflectionMultiplier = pow(metallic, refl.reflectionSpecularFalloffExponent) * screenEdgefactor * -reflectionDirection.z;

				reflColor.rgb *= clamp(reflectionMultiplier, 0.0, 0.9) * fresnel;
				// Metallic factor influences the mix of albedo and SSR color
				outFragcolor = reflColor;
			}	
		}
	}
}