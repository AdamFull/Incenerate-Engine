#pragma once

#include "EngineEnums.h"

#include <string>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

namespace engine
{
	struct FWindowCreateInfo
	{
		int32_t width;
		int32_t height;

		std::string srName;
	};

	struct FEngineCreateInfo
	{
		ERenderApi eAPI;
		FWindowCreateInfo window;
	};

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

        const glm::vec3& getPosition() const { return pos; }
        void setPosition(const glm::vec3& position) { pos = position; }

        const glm::vec3& getRotation() const { return rot; }
        void setRotation(const glm::vec3& rotation) { rot = rotation; }

        const glm::vec3& getScale() const { return scale; }
        void setScale(const glm::vec3& _scale) { scale = _scale; }

        /**
         * @brief Get render object model matrix
         *
         * @return glm::mat4 Model matrix
         */
        const glm::mat4 getModel()
        {
            glm::mat4 model{ 1.0 };

            model = glm::translate(model, pos);

            if (rot.x != 0)
                model = glm::rotate(model, rot.x, glm::vec3(1.0, 0.0, 0.0));
            if (rot.y != 0)
                model = glm::rotate(model, rot.y, glm::vec3(0.0, 1.0, 0.0));
            if (rot.z != 0)
                model = glm::rotate(model, rot.z, glm::vec3(0.0, 0.0, 1.0));

            model = glm::scale(model, scale);

            return model;
        }
    };

    void to_json(nlohmann::json& json, const FTransform& type);
    void from_json(const nlohmann::json& json, FTransform& type);
}