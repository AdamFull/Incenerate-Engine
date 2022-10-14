#include "Camera.h"

using namespace engine;

void CCamera::create()
{
	//angleV = glm::degrees(-transform.rot.y);
	//angleH = glm::degrees(glm::atan(transform.rot.z / transform.rot.x));
}

void CCamera::update(float fDT)
{
    dt = fDT;
    viewPos = glm::vec4(transform.pos, 0.0);

    auto view = getView();
    auto projection = getProjection();
    //frustum.update(view, projection);
}

void CCamera::moveForward(bool bInv)
{
    float dir = bInv ? -1.f : 1.f;
    transform.pos += getForwardVector() * dir * dt * sensitivity;
}

void CCamera::moveRight(bool bInv)
{
    float dir = bInv ? -1.f : 1.f;
    transform.pos += getRightVector() * dir * dt * sensitivity;
}

void CCamera::moveUp(bool bInv)
{
    float dir = bInv ? -1.f : 1.f;
    transform.pos += getUpVector() * dir * dt * sensitivity;
}

void CCamera::lookAt(float dX, float dY)
{
    float rotX = dX / dt;
    float rotY = dY / dt;

    angleH += rotX * sensitivity;
    if (angleV + rotY * sensitivity > 89)
        angleV = 89;
    else if (angleV + rotY * sensitivity < -89)
        angleV = -89;
    else
        angleV += rotY * sensitivity;

    const float w{ cos(glm::radians(angleV)) * -cos(glm::radians(angleH)) };
    const float u{ -sin(glm::radians(angleV)) };
    const float v{ cos(glm::radians(angleV)) * -sin(glm::radians(angleH)) };

    transform.rot = glm::normalize(glm::vec3(w * -1.f, u, v * -1.f));
}

glm::mat4 CCamera::getProjection(bool flipY) const
{
    auto perspective = glm::perspective(glm::radians(fieldOfView), aspect, nearPlane, farPlane);
    if (flipY)
        perspective[1][1] *= -1.f;
    return perspective;
}

glm::mat4 CCamera::getView(bool flipY) const
{
    auto position = transform.pos;
    if (flipY)
        position.y *= -1.f;

    return glm::lookAt(position, position + transform.rot, getUpVector());
}

glm::vec3 CCamera::getForwardVector() const
{
    return transform.rot;
}

glm::vec3 CCamera::getUpVector() const
{
    return glm::vec3{ 0.0, 1.0, 0.0 };
}

glm::vec3 CCamera::getRightVector() const
{
    return glm::normalize(glm::cross(getForwardVector(), getUpVector()));
}
