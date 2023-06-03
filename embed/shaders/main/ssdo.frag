#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : require

#define VK_FRAGMENT_SHADER

//--------------------Includes--------------------
#include "../shader_util.glsl"
#include "../math.glsl"

const int samplesCount = 32;
const int indirectAmount = 4;
const int noiseAmount = 2;

//--------------------Texture bindings--------------------
layout (set = 1, binding = 0) uniform sampler2D albedo_tex;
layout (set = 1, binding = 1) uniform sampler2D depth_tex;
layout (set = 1, binding = 2) uniform sampler2D ssdo_noise_tex;

//--------------------In/Out locations--------------------
layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragcolor;

//--------------------Uniforms--------------------
layout (std140, binding = 0) uniform UBODirectionalOcclusion
{
	vec4 samples[SSDO_KERNEL_SIZE];
    mat4 projection;
    mat4 invProjection;
	float zNear;
	float zFar;
	float radius;
	float bias;
} ubo;

vec2 mod_dither3(vec2 u) 
{
    float noiseX = mod(u.x + u.y + mod(208. + u.x * 3.58, 13. + mod(u.y * 22.9, 9.)), 7.) * .143;
    float noiseY = mod(u.y + u.x + mod(203. + u.y * 3.18, 12. + mod(u.x * 27.4, 8.)), 6.) * .139;
    return vec2(noiseX, noiseY) * 2.0 - 1.0;
}

vec2 dither(vec2 coord, float seed, vec2 size) 
{
    float noiseX = ((fract(1.0 - (coord.x + seed * 1.0) * (size.x / 2.0)) * 0.25) + (fract((coord.y + seed * 2.0) * (size.y / 2.0)) * 0.75)) * 2.0 - 1.0;
    float noiseY = ((fract(1.0 - (coord.x + seed * 3.0) * (size.x / 2.0)) * 0.75) + (fract((coord.y + seed * 4.0) * (size.y / 2.0)) * 0.25)) * 2.0 - 1.0;
    return vec2(noiseX, noiseY);
}

vec3 getViewPos(sampler2D tex, vec2 coord, mat4 ipm) 
{
    float depth = texture(tex, coord).r;

    //Turn the current pixel from ndc to world coordinates
    vec3 pixel_pos_ndc = vec3(coord * 2.0 - 1.0, depth * 2.0 - 1.0);
    vec4 pixel_pos_clip = ipm * vec4(pixel_pos_ndc, 1.0);
    vec3 pixel_pos_cam = pixel_pos_clip.xyz / pixel_pos_clip.w;
    return pixel_pos_cam;
}

vec3 getViewNormal(sampler2D tex, vec2 coord, mat4 ipm)
{
	vec2 texelSize = 1.0 / vec2(textureSize(tex, 0));
    float pW = texelSize.x;
    float pH = texelSize.y;

    vec3 p1 = getViewPos(tex, coord + vec2(pW, 0.0), ipm).xyz;
    vec3 p2 = getViewPos(tex, coord + vec2(0.0, pH), ipm).xyz;
    vec3 p3 = getViewPos(tex, coord + vec2(-pW, 0.0), ipm).xyz;
    vec3 p4 = getViewPos(tex, coord + vec2(0.0, -pH), ipm).xyz;

    vec3 vP = getViewPos(tex, coord, ipm);

    vec3 dx = vP - p1;
    vec3 dy = p2 - vP;
    vec3 dx2 = p3 - vP;
    vec3 dy2 = vP - p4;

    if (length(dx2) < length(dx) && coord.x - pW >= 0.0 || coord.x + pW > 1.0) 
    {
        dx = dx2;
    }

    if (length(dy2) < length(dy) && coord.y - pH >= 0.0 || coord.y + pH > 1.0) 
    {
        dy = dy2;
    }

    return normalize(-cross(dx, dy).xyz);
}

float lenSq(vec3 v) 
{
    return pow(v.x, 2.0) + pow(v.y, 2.0) + pow(v.z, 2.0);
}


vec3 lightSample(vec2 coord, mat4 ipm, vec2 lightcoord, vec3 normal, vec3 position, float n, vec2 texsize)
{
	vec2 random = (mod_dither3((coord * texsize) + vec2(n * 82.294, n * 127.721))) * 0.01 * noiseAmount;
	//vec2 random = dither(coord, 1.0, texsize) * 0.1 * noiseAmount;

	lightcoord *= vec2(0.7);

	//light absolute data
    vec3 lightcolor = texture(albedo_tex, ((lightcoord)+random)).rgb;
    vec3 lightnormal = getViewNormal(depth_tex, fract(lightcoord) + random, ipm).rgb;
    vec3 lightposition = getViewPos(depth_tex, fract(lightcoord) + random, ipm).xyz;

	//light variable data
    vec3 lightpath = lightposition - position;
    vec3 lightdir = normalize(lightpath);

	//falloff calculations
    float cosemit = clamp(dot(lightdir, -lightnormal), 0.0, 1.0); //emit only in one direction
    float coscatch = clamp(dot(lightdir, normal) * 0.5 + 0.5, 0.0, 1.0); //recieve light from one direction
    float distfall = pow(lenSq(lightpath), 0.1) + 1.0;        //fall off with distance

	return (lightcolor * cosemit * coscatch / distfall) * (length(lightposition) / 20.0);
}

void main()
{
	vec3 position = getViewPos(depth_tex, inUV, ubo.invProjection);
	vec3 normal = getViewNormal(depth_tex, inUV, ubo.invProjection);

	//vec3 normal = getNormalFromDepth(depth_tex, inUV, ubo.invProjection);
	//
	//vec3 pos = vec3(inUV, texture(depth_tex, inUV).r);
	//vec3 NDC_POS = (2.0 * pos) - 1.0;
	//vec4 unprojectPosition = ubo.invProjection * vec4(NDC_POS, 1.0);
    //vec3 viewPosition = unprojectPosition.xyz / unprojectPosition.w;

	float dlong = PI * (3.0 - sqrt(5.0));
    float dz = 1.0 / float(samplesCount);
    float l = 0.0;
    float z = 1.0 - dz / 2.0;

	vec3 direct = texture(albedo_tex, inUV).rgb;
	vec3 indirect = vec3(0.f);

	vec2 texSize = textureSize(albedo_tex, 0);

	for (int i = 0; i < samplesCount; i++)
    {
		float r = sqrt(1.0 - z);

        float xpoint = (cos(l) * r) * 0.5 + 0.5;
		float ypoint = (sin(l) * r) * 0.5 + 0.5;

		z = z - dz;
		l = l + dlong;

		indirect += lightSample(inUV, ubo.invProjection, vec2(xpoint, ypoint), normal, position, float(i), texSize);
	}

	outFragcolor = vec4(direct + (indirect / float(samplesCount) * indirectAmount), 1.f);
	
    // Get a random vector using a noise lookup
	//ivec2 texDim = textureSize(depth_tex, 0); 
	//ivec2 noiseDim = textureSize(ssdo_noise_tex, 0);
	//const vec2 noiseUV = vec2(float(texDim.x)/float(noiseDim.x), float(texDim.y)/(noiseDim.y)) * inUV;  
	//vec3 randomVec = texture(ssdo_noise_tex, noiseUV).xyz * 2.0 - 1.0;
	//
	//// Create TBN matrix
	//vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	//vec3 bitangent = cross(normal, tangent);
	//mat3 TBN = mat3(tangent, bitangent, normal);
	//
	//// Calculate occlusion value
	//vec4 occlusion = vec4(0.0f, 0.0f, 0.0f, 1.f);
	//for(int i = 0; i < SSDO_KERNEL_SIZE; i++)
	//{		
	//	vec3 samplePos = TBN * ubo.samples[i].xyz; 
	//	samplePos = viewPosition + samplePos * ubo.radius; 
	//	
	//	// project
	//	vec4 offset = vec4(samplePos, 1.0f);
	//	offset = ubo.projection * offset; 
	//	offset.xyz /= offset.w; 
	//	offset.xyz = offset.xyz * 0.5f + 0.5f;
	//	
	//	float sampleDepth = -getLinearDepth(depth_tex, offset.xy, ubo.zNear, ubo.zFar);
	//
	//	float rangeCheck = smoothstep(0.0f, 1.0f, ubo.radius / abs(viewPosition.z - sampleDepth));
	//	if(sampleDepth >= samplePos.z + ubo.bias)
	//		occlusion.rgb += texture(albedo_tex, offset.xy).rgb;       
	//}
	//occlusion = 1.0 - (occlusion / float(SSDO_KERNEL_SIZE));
	//
    //outFragcolor = occlusion;
}