#include "Camera.h"

#include "system/window/WindowHandle.h"

using namespace engine;
using namespace engine::system::window;

void CCamera::create()
{
    transform.pos = { -6.3434, 15.9819, -22.8122 };
    transform.rot = { 0.514259, -0.318132, 0.796448 };
	angleV = glm::degrees(-transform.rot.y);
	angleH = glm::degrees(glm::atan(transform.rot.z / transform.rot.x));
}

void CCamera::update(float fDT)
{
    dt = fDT;
    viewPos = glm::vec4(transform.pos, 1.0);

    onResize(CWindowHandle::iWidth, CWindowHandle::iHeight);

    if (bMoved)
    {
        recalculateView();
        //recalculateRayDirections();
        bMoved = false;
    }

    frustum.update(view, projection);
}

void CCamera::onResize(uint32_t width, uint32_t height)
{
    if (width == this->width && height == this->height)
        return;

    this->width = width;
    this->height = height;

    recalculateProjection();
    //recalculateRayDirections();
}

void CCamera::moveForward(bool bInv)
{
    float dir = bInv ? -1.f : 1.f;
    transform.pos += getForwardVector() * dir * dt * sensitivity;
    bMoved = true;
}

void CCamera::moveRight(bool bInv)
{
    float dir = bInv ? -1.f : 1.f;
    transform.pos += getRightVector() * dir * dt * sensitivity;
    bMoved = true;
}

void CCamera::moveUp(bool bInv)
{
    float dir = bInv ? -1.f : 1.f;
    transform.pos += getUpVector() * dir * dt * sensitivity;
    bMoved = true;
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

    auto newrot = glm::normalize(glm::vec3(w * -1.f, u, v * -1.f));
    if (transform.rot != newrot)
    {
        transform.rot = newrot;
        bMoved = true;
    }
}

const glm::mat4& CCamera::getProjection(bool flipY) const
{
    return projection;
}

const glm::mat4& CCamera::getView(bool flipY) const
{
    return view;
}

glm::vec3& CCamera::getViewPos()
{
    return transform.pos;
}

glm::vec3 CCamera::getForwardVector() const
{
    return glm::normalize(transform.rot);
}

glm::vec3 CCamera::getUpVector() const
{
    return glm::normalize(glm::cross(getRightVector(), getForwardVector()));
}

glm::vec3 CCamera::getRightVector() const
{
    return glm::normalize(glm::cross(getForwardVector(), glm::vec3{ 0.0, 1.0, 0.0 }));
}

void CCamera::recalculateView()
{
    view = glm::lookAt(transform.pos, transform.pos + getForwardVector(), getUpVector());
    invView = glm::inverse(view);
}

void CCamera::recalculateProjection()
{
    projection = glm::perspective(glm::radians(fieldOfView), aspect, nearPlane, farPlane);
    invProjection = glm::inverse(projection);
}

void CCamera::recalculateRayDirections()
{
    constexpr const uint32_t divider = 4;
    vRayDirections.resize(width * height / divider);
    float fwidth{ (float)width }, fheight{ (float)height };

    for (uint32_t y = 0; y < height; y++)
    {
        if (y % divider == 0)
        {
            float fy = (float)y;
            for (uint32_t x = 0; x < width; x++)
            {
                if (x % divider == 0)
                {
                    float fx = (float)x;

                    glm::vec2 coord = { fx / fwidth, fy / fheight };
                    coord = coord * 2.0f - 1.0f;

                    glm::vec4 target = invProjection * glm::vec4(coord.x, coord.y, 1, 1);
                    glm::vec3 rayDirection = glm::vec3(invView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space
                    auto idx = (x + y * width) / divider;
                    vRayDirections[idx] = rayDirection;
                }
            }
        }
    }
}