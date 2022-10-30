#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility/uparse.hpp>

namespace engine
{
    namespace game
    {
        struct FTransform
        {
            FTransform() = default;
            glm::vec3 pos{};
            glm::vec3 rot{};
            glm::vec3 scale{ 1.f, 1.f, 1.f };

            FTransform& operator+=(const FTransform& rhs)
            {
                this->scale *= rhs.scale;
                this->pos = this->pos * rhs.scale + rhs.pos;
                this->rot += rhs.rot;
                return *this;
            }

            const glm::vec3& getPosition() const;
            void setPosition(const glm::vec3& position);

            const glm::vec3& getRotation() const;
            void setRotation(const glm::vec3& rotation);

            const glm::vec3& getScale() const;
            void setScale(const glm::vec3& _scale);

            /**
             * @brief Get render object model matrix
             *
             * @return glm::mat4 Model matrix
             */
            const glm::mat4 getModel();
        };

        void to_json(nlohmann::json& json, const FTransform& type);
        void from_json(const nlohmann::json& json, FTransform& type);
    }
}