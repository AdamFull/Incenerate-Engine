#ifndef MATH_LIB
#define MATH_LIB

//Here is implementation of math functions/constanst that repeating in code
#define PI 3.1415926535897932384626433832795

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
    if (b * b - 4.0 * a * c < 0.0) {
        return vec2(-1.0, -1.0);
    }

    float t1 = (-b - sqrt((b * b) - 4.0 * a * c)) / (2.0 * a);
    float t2 = (-b + sqrt((b * b) - 4.0 * a * c)) / (2.0 * a);
    return vec2(t1, t2);
}

float remap(float value, float minValue, float maxValue, float newMinValue, float newMaxValue)
{
	return newMinValue + (value - minValue) / (maxValue - minValue) * (newMaxValue - newMinValue);
}

vec4 quatConj(vec4 q)
{
    return vec4(-q.x, -q.y, -q.z, q.w);
}

vec4 quatMult(vec4 q1, vec4 q2)
{
    vec4 qr;
    qr.x = (q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y);
    qr.y = (q1.w * q2.y) - (q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x);
    qr.z = (q1.w * q2.z) + (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w);
    qr.w = (q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z);
    return qr;
}

vec4 quatAxisAngle(vec3 axis, float angleRad)
{
    const float halfAngle = angleRad / 2.0f;

    vec4 q = vec4(axis.x * sin(halfAngle),
        axis.y * sin(halfAngle),
        axis.z * sin(halfAngle),
        cos(halfAngle));

    return q;
}

vec3 rotate(vec3 v, vec4 quat)
{
    vec4 qr = quat;
    vec4 qr_conj = quatConj(qr);
    vec4 q_pos = vec4(v.x, v.y, v.z, 0);

    vec4 q_tmp = quatMult(qr, q_pos);
    qr = quatMult(q_tmp, qr_conj);

    return vec3(qr.x, qr.y, qr.z);
}

vec3 rotate(vec3 v, vec3 axis, float angle)
{
    vec4 quat = quatAxisAngle(axis, angle);

    return rotate(v, quat);
}

vec4 linearTosRGB(vec4 linearRGB)
{
    bvec4 cutoff = lessThan(linearRGB, vec4(0.0031308));
    vec4 higher = vec4(1.055) * pow(linearRGB, vec4(1.0 / 2.4)) - vec4(0.055);
    vec4 lower = linearRGB * vec4(12.92);

    return mix(higher, lower, cutoff);
}

#endif
