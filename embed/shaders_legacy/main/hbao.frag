#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

#define VK_FRAGMENT_SHADER

//--------------------Includes--------------------
#include "../shader_util.glsl"
#include "../math.glsl"

#define RADIUS 1.0
#define NUMBER_OF_SAMPLING_DIRECTIONS 6
#define STEP 0.004
#define NUMBER_OF_STEPS 4
#define TANGENT_BIAS 0.3
#define STRENGTH 2.0

//--------------------Texture bindings--------------------
layout (set = 1, binding = 0) uniform sampler2D depth_tex;
layout (set = 1, binding = 1) uniform sampler2D ssao_noise_tex;

//--------------------In/Out locations--------------------
layout (location = 0) in vec2 inUV;
layout (location = 0) out float outFragcolor;

//--------------------Uniforms--------------------
layout (std140, binding = 0) uniform UBOAmbientOcclusion
{
    mat4 invProjection;
} ubo;

void main()
{
	float sum = 0.0;
	float occlusion = 0.0;

    vec3 pos = vec3(inUV, texture(depth_tex, inUV).r);
    vec3 NDC_POS = (2.0 * pos) - 1.0;
	vec4 unprojectPosition = ubo.invProjection * vec4(NDC_POS, 1.0);
    vec3 viewPosition = unprojectPosition.xyz / unprojectPosition.w;

    vec3 normal = getNormalFromDepth(depth_tex, inUV, ubo.invProjection);

    ivec2 texDim = textureSize(depth_tex, 0); 
	ivec2 noiseDim = textureSize(ssao_noise_tex, 0);
	const vec2 noiseUV = vec2(float(texDim.x)/float(noiseDim.x), float(texDim.y)/(noiseDim.y)) * inUV;  
	vec3 randomVec = texture(ssao_noise_tex, noiseUV).xyz * 2.0 - 1.0;
	
	mat2 rotationMatrix = mat2(randomVec.x, -randomVec.y, randomVec.y, randomVec.x);
	
	float samplingDiskDirection = 2 * PI / NUMBER_OF_SAMPLING_DIRECTIONS;
	
	for(int i = 0; i < NUMBER_OF_SAMPLING_DIRECTIONS; i++)
	{
		float samplingDirectionAngle = i * samplingDiskDirection;
	
		vec2 samplingDirection = rotationMatrix * vec2(cos(samplingDirectionAngle), sin(samplingDirectionAngle));
	
		float tangentAngle = acos(dot(vec3(samplingDirection, 0.0), normal.xyz)) - (0.5 * PI) + TANGENT_BIAS;
        float horizonAngle = tangentAngle; //set the horizon angle to the tangent angle to begin with
	
		vec3 LastDifference = vec3(0);
	
		for(int j = 0; j < NUMBER_OF_STEPS; j++)
		{
			vec2 stepForward = float(j+1) * STEP * samplingDirection;
	
			vec2 stepPosition = inUV + stepForward;
	
			float steppedLocationZ = texture(depth_tex, stepPosition.st).r;
			vec3 steppedLocationPosition = vec3(stepPosition, steppedLocationZ);
	
			vec3 steppedPositionNDC = (2.0 * steppedLocationPosition) - 1.0;
            vec4 SteppedPositionUnProj = ubo.invProjection * vec4(steppedPositionNDC, 1.0);
            vec3 viewSpaceSteppedPosition = SteppedPositionUnProj.xyz / SteppedPositionUnProj.w;
	
			vec3 diff = viewSpaceSteppedPosition.xyz - viewPosition;
			if(length(diff) < RADIUS)
			{
 	
                LastDifference = diff;
                float FoundElevationAngle = atan(diff.z / length(diff.xy));
                horizonAngle = max(horizonAngle, FoundElevationAngle);
            }
		}
	
		float norm = length(LastDifference) / RADIUS;
        float attenuation = 1 - norm * norm;
 	
        occlusion = clamp(attenuation * (sin(horizonAngle) - sin(tangentAngle)), 0.0, 1.0);
        sum += 1.0 - occlusion;
	}
	
	sum /= NUMBER_OF_SAMPLING_DIRECTIONS;
	outFragcolor = sum;
}