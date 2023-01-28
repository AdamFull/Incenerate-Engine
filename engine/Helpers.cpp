#include "Helpers.h"

namespace engine
{
    glm::vec4 encodeIdToColor(uint32_t hex)
    {
        glm::vec4 color;
        color.x = static_cast<float>((hex & 0xff000000) >> 24);
        color.y = static_cast<float>((hex & 0x00ff0000) >> 16);
        color.z = static_cast<float>((hex & 0x0000ff00) >> 8);
        color.w = static_cast<float>(hex & 0x000000ff);
        return color / 255.f;
    }

    uint32_t decodeIdFromColor(uint8_t x, uint8_t y, uint8_t z, uint8_t w)
    {
        return ((x & 0xff) << 24) + ((y & 0xff) << 16) + ((z & 0xff) << 8) + (w & 0xff);
    }
}