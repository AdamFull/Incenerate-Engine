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

	bool decompose(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
	{
		using T = float;
		glm::mat4 LocalMatrix(transform);

		// Normalize the matrix.
		if (glm::epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), glm::epsilon<T>()))
			return false;

		// First, isolate perspective.  This is the messiest.
		if (
			glm::epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), glm::epsilon<T>()) ||
			glm::epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), glm::epsilon<T>()) ||
			glm::epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), glm::epsilon<T>()))
		{
			// Clear the perspective partition
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
			LocalMatrix[3][3] = static_cast<T>(1);
		}

		// Next take care of translation (easy).
		translation = glm::vec3(LocalMatrix[3]);
		LocalMatrix[3] = glm::vec4(0, 0, 0, LocalMatrix[3].w);

		glm::vec3 Row[3], Pdum3;

		// Now get scale and shear.
		for (glm::length_t i = 0; i < 3; ++i)
			for (glm::length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = glm::length(Row[0]);
		Row[0] = glm::detail::scale(Row[0], static_cast<T>(1));
		scale.y = glm::length(Row[1]);
		Row[1] = glm::detail::scale(Row[1], static_cast<T>(1));
		scale.z = glm::length(Row[2]);
		Row[2] = glm::detail::scale(Row[2], static_cast<T>(1));

		rotation.y = glm::asin(-Row[0][2]);
		if (glm::cos(rotation.y) != 0)
		{
			rotation.x = atan2(Row[1][2], Row[2][2]);
			rotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else
		{
			rotation.x = atan2(-Row[2][0], Row[1][1]);
			rotation.z = 0;
		}


		return true;
	}
}