#include "Frustum.h"

using namespace engine;

void FFrustum::update(const glm::mat4& view, const glm::mat4& projection)
{
    clip[0] = view[0][0] * projection[0][0] + view[0][1] * projection[1][0] + view[0][2] * projection[2][0] + view[0][3] * projection[3][0];
    clip[1] = view[0][0] * projection[0][1] + view[0][1] * projection[1][1] + view[0][2] * projection[2][1] + view[0][3] * projection[3][1];
    clip[2] = view[0][0] * projection[0][2] + view[0][1] * projection[1][2] + view[0][2] * projection[2][2] + view[0][3] * projection[3][2];
    clip[3] = view[0][0] * projection[0][3] + view[0][1] * projection[1][3] + view[0][2] * projection[2][3] + view[0][3] * projection[3][3];

    clip[4] = view[1][0] * projection[0][0] + view[1][1] * projection[1][0] + view[1][2] * projection[2][0] + view[1][3] * projection[3][0];
    clip[5] = view[1][0] * projection[0][1] + view[1][1] * projection[1][1] + view[1][2] * projection[2][1] + view[1][3] * projection[3][1];
    clip[6] = view[1][0] * projection[0][2] + view[1][1] * projection[1][2] + view[1][2] * projection[2][2] + view[1][3] * projection[3][2];
    clip[7] = view[1][0] * projection[0][3] + view[1][1] * projection[1][3] + view[1][2] * projection[2][3] + view[1][3] * projection[3][3];

    clip[8] = view[2][0] * projection[0][0] + view[2][1] * projection[1][0] + view[2][2] * projection[2][0] + view[2][3] * projection[3][0];
    clip[9] = view[2][0] * projection[0][1] + view[2][1] * projection[1][1] + view[2][2] * projection[2][1] + view[2][3] * projection[3][1];
    clip[10] = view[2][0] * projection[0][2] + view[2][1] * projection[1][2] + view[2][2] * projection[2][2] + view[2][3] * projection[3][2];
    clip[11] = view[2][0] * projection[0][3] + view[2][1] * projection[1][3] + view[2][2] * projection[2][3] + view[2][3] * projection[3][3];

    clip[12] = view[3][0] * projection[0][0] + view[3][1] * projection[1][0] + view[3][2] * projection[2][0] + view[3][3] * projection[3][0];
    clip[13] = view[3][0] * projection[0][1] + view[3][1] * projection[1][1] + view[3][2] * projection[2][1] + view[3][3] * projection[3][1];
    clip[14] = view[3][0] * projection[0][2] + view[3][1] * projection[1][2] + view[3][2] * projection[2][2] + view[3][3] * projection[3][2];
    clip[15] = view[3][0] * projection[0][3] + view[3][1] * projection[1][3] + view[3][2] * projection[2][3] + view[3][3] * projection[3][3];

    //Left
    frustumSides[1][0] = clip[3] - clip[0];
    frustumSides[1][1] = clip[7] - clip[4];
    frustumSides[1][2] = clip[11] - clip[8];
    frustumSides[1][3] = clip[15] - clip[12];
    normalizeFrustumSide(1);

    //Right
    frustumSides[0][0] = clip[3] + clip[0];
    frustumSides[0][1] = clip[7] + clip[4];
    frustumSides[0][2] = clip[11] + clip[8];
    frustumSides[0][3] = clip[15] + clip[12];
    normalizeFrustumSide(0);

    //Top
    frustumSides[3][0] = clip[3] - clip[1];
    frustumSides[3][1] = clip[7] - clip[5];
    frustumSides[3][2] = clip[11] - clip[9];
    frustumSides[3][3] = clip[15] - clip[13];
    normalizeFrustumSide(3);

    //Bottom
    frustumSides[2][0] = clip[3] + clip[1];
    frustumSides[2][1] = clip[7] + clip[5];
    frustumSides[2][2] = clip[11] + clip[9];
    frustumSides[2][3] = clip[15] + clip[13];
    normalizeFrustumSide(2);

    //Back
    frustumSides[4][0] = clip[3] + clip[2];
    frustumSides[4][1] = clip[7] + clip[6];
    frustumSides[4][2] = clip[11] + clip[10];
    frustumSides[4][3] = clip[15] + clip[14];
    normalizeFrustumSide(4);

    //Front
    frustumSides[5][0] = clip[3] - clip[2];
    frustumSides[5][1] = clip[7] - clip[6];
    frustumSides[5][2] = clip[11] - clip[10];
    frustumSides[5][3] = clip[15] - clip[14];
}

bool FFrustum::checkPoint(const glm::vec3& pos) const
{
    for (uint32_t i = 0; i < 6; i++)
    {
        if (frustumSides[i][0] * pos.x + frustumSides[i][1] * pos.y + frustumSides[i][2] * pos.z + frustumSides[i][3] <= 0.0f)
            return false;
    }
    return true;
}

bool FFrustum::checkSphere(const glm::vec3& pos, float radius) const
{
    for (uint32_t i = 0; i < 6; i++)
    {
        if (frustumSides[i][0] * pos.x + frustumSides[i][1] * pos.y + frustumSides[i][2] * pos.z + frustumSides[i][3] <= -radius)
            return false;
    }
    return true;
}

bool FFrustum::checkBox(const glm::vec3& start, const glm::vec3& end) const
{
    for (uint32_t i = 0; i < 6; i++)
    {
        if (frustumSides[i][0] * start.x + frustumSides[i][1] * start.y + frustumSides[i][2] * start.z + frustumSides[i][3] <= 0.0f
            && frustumSides[i][0] * end.x + frustumSides[i][1] * start.y + frustumSides[i][2] * start.z + frustumSides[i][3] <= 0.0f
            && frustumSides[i][0] * start.x + frustumSides[i][1] * end.y + frustumSides[i][2] * start.z + frustumSides[i][3] <= 0.0f
            && frustumSides[i][0] * end.x + frustumSides[i][1] * end.y + frustumSides[i][2] * start.z + frustumSides[i][3] <= 0.0f
            && frustumSides[i][0] * start.x + frustumSides[i][1] * start.y + frustumSides[i][2] * end.z + frustumSides[i][3] <= 0.0f
            && frustumSides[i][0] * end.x + frustumSides[i][1] * start.y + frustumSides[i][2] * end.z + frustumSides[i][3] <= 0.0f
            && frustumSides[i][0] * start.x + frustumSides[i][1] * end.y + frustumSides[i][2] * end.z + frustumSides[i][3] <= 0.0f
            && frustumSides[i][0] * end.x + frustumSides[i][1] * end.y + frustumSides[i][2] * end.z + frustumSides[i][3] <= 0.0f)
            return false;
    }
    return true;
}

void FFrustum::normalizeFrustumSide(size_t side)
{
    auto magnitude = std::sqrt(frustumSides[side][0] * frustumSides[side][0] + frustumSides[side][1] * frustumSides[side][1] + frustumSides[side][2] * frustumSides[side][2]);
    frustumSides[side][0] /= magnitude;
    frustumSides[side][1] /= magnitude;
    frustumSides[side][2] /= magnitude;
    frustumSides[side][3] /= magnitude;
}