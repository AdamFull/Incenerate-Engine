#pragma once

#include <glm/glm.hpp>
#include <array>

namespace engine
{
    struct FFrustum
    {
    public:
        void update(const glm::mat4& view, const glm::mat4& projection);

        /**
         * @brief Check is point in frustum
         *
         * @param pos Point position
         * @return true when point is in frustum
         */
        bool checkPoint(const glm::vec3& pos) const;

        /**
         * @brief Check is sphere intersects with frustum
         *
         * @param pos Sphere center position
         * @param radius Sphere radius
         * @return true when sphere intersects with camera frustum
         */
        bool checkSphere(const glm::vec3& pos, float radius) const;

        /**
         * @brief Check is bounding box intersects with frustum
         *
         * @param start Bounding box start point
         * @param end Bounding box end boint
         * @return true when bounding box intersects with camera frustum
         */
        bool checkBox(const glm::vec3& start, const glm::vec3& end) const;

        const std::array<std::array<float, 4>, 6>& getFrustumSides() const { return frustumSides; }
    private:
        void normalizeFrustumSide(size_t side);

        std::array<std::array<float, 4>, 6> frustumSides{};
        std::array<float, 16> clip;
    };
}