#ifndef SHADER_UTIL
#define SHADER_UTIL

// From http://filmicworlds.com/blog/filmic-tonemapping-operators/
vec3 Uncharted2Tonemap(vec3 color)
{
	float A = 0.15f;
	float B = 0.50f;
	float C = 0.10f;
	float D = 0.20f;
	float E = 0.02f;
	float F = 0.30f;
	float W = 11.2f;
	return ((color*(A*color+C*B)+D*E)/(color*(A*color+B)+D*F))-E/F;
}

vec3 getTangentSpaceNormalMap(sampler2D samplerNormal, mat3 TBN, vec2 uv, float scale)
{
	vec3 normalColor = texture(samplerNormal, uv).rgb;
    normalColor = normalize(TBN * ((2.0 * normalColor - 1.0) * vec3(scale, scale, 1.0)));
	return normalColor;
}

// Calculate normal in tangent space
mat3 calculateTBN(vec3 normal, vec4 tangent)
{
	vec3 N = normalize(normal);
	vec3 T = normalize(tangent.xyz);
    vec3 B = normalize(cross(N, T) * tangent.w);
	
	return mat3(T, B, N);
}

vec3 getPositionFromDepth(vec2 texcoord, float depth, mat4 invViewProjection)
{
    vec4 clipSpaceLocation;
    clipSpaceLocation.xy = texcoord * 2.0f - 1.0f;
    clipSpaceLocation.z = depth;
    clipSpaceLocation.w = 1.0f;
    vec4 homogenousLocation = invViewProjection * clipSpaceLocation;
    return homogenousLocation.xyz / homogenousLocation.w;
}

vec3 WorldPosFromDepth(vec2 texcoord, float depth, mat4 invProjMatrix, mat4 invViewMatrix) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(texcoord * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invProjMatrix * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = invViewMatrix * viewSpacePosition;

    return worldSpacePosition.xyz;
}

uvec4 packTextures(vec3 normal_map, vec3 albedo_map, vec4 pbr_map)
{
    uvec4 texture_pack;
    //Storing RG components of normal map in 32bit R component
    texture_pack.r = packHalf2x16(normal_map.rg);
    //Storing B component of normal map and R component of albedo in G
    texture_pack.g = packHalf2x16(vec2(normal_map.b, albedo_map.r));
    //Storing GB components of albedo in B
    texture_pack.b = packHalf2x16(albedo_map.gb);
    // Store metal, roughness, ao and heignt in A component
    texture_pack.a = packUnorm4x8(pbr_map);
    return texture_pack;
}

void unpackTextures(in uvec4 texture_pack, out vec3 normal_map, out vec3 albedo_map, out vec4 pbr_map)
{
    vec2 pack_R = unpackHalf2x16(texture_pack.r);
	vec2 pack_G = unpackHalf2x16(texture_pack.g);
	vec2 pack_B = unpackHalf2x16(texture_pack.b);
	pbr_map = unpackUnorm4x8(texture_pack.a);
    normal_map = vec3(pack_R, pack_G.r);
    albedo_map = vec3(pack_G.g, pack_B);
}

vec4 per_object_motion_blir(sampler2D velocity_texture, sampler2D color_texture, vec2 inUV, float velocityScale, int max_samples)
{
    vec2 texelSize = 1.0 / vec2(textureSize(color_texture, 0));
    vec2 velocity = texture(velocity_texture, inUV * vec2(1.0, -1.0)).rg * velocityScale;
    float speed = length(velocity / texelSize);
    int samples = clamp(int(speed), 1, max_samples);

    vec4 motionBlur = texture(color_texture, inUV);
    for (int i = 1; i < samples; ++i) 
    {
        vec2 offset = velocity * (float(i) / float(samples - 1) - 0.5);
        motionBlur += texture(color_texture, inUV + offset);
    }
    motionBlur /= float(samples);
    return motionBlur;
}

//Based on https://github.com/McNopper/OpenGL/blob/master/Example42/shader/fxaa.frag.glsl
vec4 fxaa(sampler2D image, vec2 texcoord, float lumaThreshold, float reduceMin, float mulReduce, float maxSpan, vec2 texelStep)
{
    vec3 rgbM = texture(image, texcoord).rgb;

    // Sampling neighbour texels. Offsets are adapted to OpenGL texture coordinates. 
	vec3 rgbNW = textureOffset(image, texcoord, ivec2(-1, 1)).rgb;
    vec3 rgbNE = textureOffset(image, texcoord, ivec2(1, 1)).rgb;
    vec3 rgbSW = textureOffset(image, texcoord, ivec2(-1, -1)).rgb;
    vec3 rgbSE = textureOffset(image, texcoord, ivec2(1, -1)).rgb;

    // see http://en.wikipedia.org/wiki/Grayscale
	const vec3 toLuma = vec3(0.299, 0.587, 0.114);

    // Convert from RGB to luma.
	float lumaNW = dot(rgbNW, toLuma);
	float lumaNE = dot(rgbNE, toLuma);
	float lumaSW = dot(rgbSW, toLuma);
	float lumaSE = dot(rgbSE, toLuma);
	float lumaM = dot(rgbM, toLuma);

    // Gather minimum and maximum luma.
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    // If contrast is lower than a maximum threshold ...
	if (lumaMax - lumaMin <= lumaMax * lumaThreshold)
	{
		// ... do no AA and return.
		return vec4(rgbM, 1.0);
	}

    // Sampling is done along the gradient.
	vec2 samplingDirection;	
	samplingDirection.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    samplingDirection.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    // Sampling step distance depends on the luma: The brighter the sampled texels, the smaller the final sampling step direction.
    // This results, that brighter areas are less blurred/more sharper than dark areas.  
    float samplingDirectionReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * mulReduce, reduceMin);

    // Factor for norming the sampling direction plus adding the brightness influence. 
	float minSamplingDirectionFactor = 1.0 / (min(abs(samplingDirection.x), abs(samplingDirection.y)) + samplingDirectionReduce);

    // Calculate final sampling direction vector by reducing, clamping to a range and finally adapting to the texture size. 
    samplingDirection = clamp(samplingDirection * minSamplingDirectionFactor, vec2(-maxSpan), vec2(maxSpan)) * texelStep;

    // Inner samples on the tab.
	vec3 rgbSampleNeg = texture(image, texcoord + samplingDirection * (1.0/3.0 - 0.5)).rgb;
	vec3 rgbSamplePos = texture(image, texcoord + samplingDirection * (2.0/3.0 - 0.5)).rgb;

    vec3 rgbTwoTab = (rgbSamplePos + rgbSampleNeg) * 0.5;  

	// Outer samples on the tab.
	vec3 rgbSampleNegOuter = texture(image, texcoord + samplingDirection * (0.0/3.0 - 0.5)).rgb;
	vec3 rgbSamplePosOuter = texture(image, texcoord + samplingDirection * (3.0/3.0 - 0.5)).rgb;

    vec3 rgbFourTab = (rgbSamplePosOuter + rgbSampleNegOuter) * 0.25 + rgbTwoTab * 0.5;   
	
	// Calculate luma for checking against the minimum and maximum value.
	float lumaFourTab = dot(rgbFourTab, toLuma);

    // Are outer samples of the tab beyond the edge ... 
	if (lumaFourTab < lumaMin || lumaFourTab > lumaMax)
		// ... yes, so use only two samples.
		return vec4(rgbTwoTab, 1.0);

	// ... no, so use four samples. 
	return vec4(rgbFourTab, 1.0);
}

#endif