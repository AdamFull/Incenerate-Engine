#ifndef SHADER_UTIL
#define SHADER_UTIL

vec4 SRGBtoLINEAR(in vec4 srgbIn, bool srgb)
{
    if(srgb)
    {
        vec3 bLess = step(vec3(0.04045),srgbIn.xyz);
	    vec3 linOut = mix( srgbIn.xyz/vec3(12.92), pow((srgbIn.xyz+vec3(0.055))/vec3(1.055),vec3(2.4)), bLess );
	    return vec4(linOut,srgbIn.w);;
    }
    else
    {
        return srgbIn;
    }
}

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

vec3 getTangentSpaceNormalMap(in vec3 normalColor, mat3 TBN, float scale)
{
    vec3 normal = normalize(TBN * ((2.0 * normalColor - 1.0) * vec3(scale, scale, 1.0)));
	return normal;
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
    clipSpaceLocation.x = texcoord.x * 2.0f - 1.0f;
    clipSpaceLocation.y = 1.f - texcoord.y * 2.0f; // invert Y for Vulkan
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

#endif