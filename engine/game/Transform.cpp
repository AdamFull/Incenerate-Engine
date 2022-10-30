#include "Transform.h"

using namespace engine::game;

namespace engine
{
    namespace game
    {
        void to_json(nlohmann::json& json, const FTransform& type)
        {
        }

        void from_json(const nlohmann::json& json, FTransform& type)
        {

        }
    }
}

const glm::vec3& FTransform::getPosition() const 
{ 
	return pos; 
}

void FTransform::setPosition(const glm::vec3& position) 
{ 
	pos = position; 
}

const glm::vec3& FTransform::getRotation() const 
{
	return rot; 
}

void FTransform::setRotation(const glm::vec3& rotation) 
{ 
	rot = rotation; 
}

const glm::vec3& FTransform::getScale() const 
{
	return scale; 
}

void FTransform::setScale(const glm::vec3& _scale) 
{ 
	scale = _scale; 
}

const glm::mat4 FTransform::getModel()
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