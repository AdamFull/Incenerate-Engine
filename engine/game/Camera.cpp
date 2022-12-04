#include "Camera.h"

#include "Engine.h"
#include "system/window/WindowHandle.h"

using namespace engine;
using namespace engine::game;
using namespace engine::system::window;

void CCamera::create()
{
    transform.pos = { -6.3434, 15.9819, -22.8122 };
    transform.rot = { 0.514259, -0.318132, 0.796448 };
	angleV = glm::degrees(-transform.rot.y);
	angleH = glm::degrees(glm::atan(transform.rot.z / transform.rot.x));
}

void CCamera::activate()
{
    bActive = true;
    bind();
}

void CCamera::deactivate()
{
    bActive = false;
}

void CCamera::update(float fDT)
{
    dt = fDT;

    onResize(CWindowHandle::iWidth, CWindowHandle::iHeight);

    if (bMoved)
    {
        recalculateView();
        bMoved = false;
    }

    frustum.update(view, projection);
}

void CCamera::onResize(uint32_t width, uint32_t height)
{
    if (width == viewportDim.x && height == viewportDim.y)
        return;

    viewportDim.x = width;
    viewportDim.y = height;

    recalculateProjection();
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

void CCamera::bind()
{
    if (bActive)
    {
        auto& renderSystem = CEngine::getInstance()->getGraphics()->getRenderSystem();
        if (renderSystem)
        {
            renderSystem->setView(view);
            renderSystem->setProjection(projection);
            renderSystem->setViewDir(transform.pos);
            renderSystem->setViewportDim(viewportDim);
            renderSystem->setFrustum(frustum);
        }
    }
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