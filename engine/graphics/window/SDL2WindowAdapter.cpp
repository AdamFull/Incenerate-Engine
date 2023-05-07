#include "SDL2WindowAdapter.h"

#include <utility/logger/logger.h>
#include "Engine.h"
#include "SDL2KeycodeHelper.hpp"

#include <SDL_vulkan.h>

using namespace engine::graphics;
using namespace engine::ecs;

CSDL2WindowAdapter::CSDL2WindowAdapter(IEventManagerInterface* evt_ptr) :
    m_pEvents(evt_ptr)
{

}

CSDL2WindowAdapter::~CSDL2WindowAdapter()
{
	destroy();
}

void CSDL2WindowAdapter::create(const FWindowCreateInfo& createInfo)
{
    uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;

    iWidth = createInfo.width;
    iHeight = createInfo.height;

    SDL_Init(SDL_INIT_VIDEO);
    pWindow = SDL_CreateWindow(createInfo.srName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, iWidth, iHeight, flags);
    log_cerror((pWindow != nullptr), "Could not create window.");
    log_info("Created window: [name: {}, extent: {}x{}, aspect: {}]", createInfo.srName, iWidth, iHeight, getAspect());

    m_pMouseEvent = m_pEvents->makeEvent(Events::Input::Mouse);
    m_pKeyEvent = m_pEvents->makeEvent(Events::Input::Key);
}

void CSDL2WindowAdapter::destroy()
{
    if (pWindow)
    {
        SDL_DestroyWindow(pWindow);
        pWindow = nullptr;
        SDL_Quit();
        log_debug("Window destroyed.");
    }
}

bool CSDL2WindowAdapter::processEvents()
{
    bool bKeyStateChange{ false };

    if (!isMinimized())
        bWasResized = false;

    SDL_Event event;
    vWinEvents.clear();

    auto surface = SDL_GetWindowSurface(pWindow);
    iWidth = surface->w;
    iHeight = surface->h;

    while (SDL_PollEvent(&event))
    {
        SDL_PumpEvents();
        switch (event.type)
        {
        case SDL_APP_TERMINATING:
        {
            int iii = 0;
        } break;
        case SDL_QUIT:
        {
            bRunning = false;
        } break;

        case SDL_KEYUP:
        case SDL_KEYDOWN:
        {
            bKeyStateChange |= mKeys.update(keyboardCodeToEngineCode(event.key.keysym.scancode), event.key.state);
        } break;

        case SDL_TEXTINPUT:
        {
        } break;

        case SDL_CONTROLLERBUTTONUP:
        case SDL_CONTROLLERBUTTONDOWN:
        {
            bKeyStateChange |= mKeys.update(gamepadCodeToEngineCode((SDL_GameControllerButton)event.cbutton.button), event.cbutton.state);
        } break;

        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
        {
            bKeyStateChange |= mKeys.update(mouseCodeToEngineCode(event.button.button), event.button.state);
        };

        case SDL_MOUSEWHEEL:
        {
        } break;

        case SDL_FINGERMOTION:
        {
        } break;

        case SDL_CONTROLLERTOUCHPADMOTION:
        {
            static float controller_xold{ 0.f }, controller_yold{ 0.f };

            controller_xold = event.ctouchpad.x;
            controller_yold = event.ctouchpad.y;

        } break;

        case SDL_MOUSEMOTION:
        {
            m_pMouseEvent->setParam(Events::Input::MouseX, static_cast<float>(event.motion.x));
            m_pMouseEvent->setParam(Events::Input::MouseY, static_cast<float>(event.motion.y));
            m_pEvents->sendEvent(m_pMouseEvent);
        } break;

        case SDL_FINGERDOWN:
        case SDL_FINGERUP:
        {
        } break;

        case SDL_CONTROLLERTOUCHPADDOWN:
        case SDL_CONTROLLERTOUCHPADUP:
        {
        } break;

        case SDL_CONTROLLERAXISMOTION:
        {
            event.caxis.type;
        } break;

        case SDL_WINDOWEVENT:
        {
            Uint8 window_event = event.window.event;


        } break;
        case SDL_WINDOWEVENT_RESIZED:
        {
            bWasResized = true;
            SDL_GetWindowSize(pWindow, &iWidth, &iHeight);
        } break;
        }

        vWinEvents.emplace_back(event);
    }

    //if (bKeyStateChange)
    {
        m_pKeyEvent->setParam(Events::Input::Key, mKeys);
        m_pEvents->sendEvent(m_pKeyEvent);
    }

    return bRunning;
}

void CSDL2WindowAdapter::createSurface(const vk::Instance& instance, vk::SurfaceKHR& surface, vk::AllocationCallbacks* allocator)
{
    VkSurfaceKHR rawSurfaceKHR{ VK_NULL_HANDLE };
    SDL_Vulkan_CreateSurface(pWindow, instance, &rawSurfaceKHR);
    surface = rawSurfaceKHR;
}

std::vector<const char*> CSDL2WindowAdapter::getWindowExtensions(bool validation)
{
    uint32_t extensionCount{ 0 };
    std::vector<const char*> extensions{};

    if (SDL_Vulkan_GetInstanceExtensions(pWindow, &extensionCount, nullptr))
    {
        extensions.resize(extensionCount);
        SDL_Vulkan_GetInstanceExtensions(pWindow, &extensionCount, extensions.data());
    }

    if (validation)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

bool CSDL2WindowAdapter::isMinimized()
{
    auto flags = SDL_GetWindowFlags(pWindow);
    return flags & SDL_WINDOW_MINIMIZED;
}

bool CSDL2WindowAdapter::wasResized()
{
    return bWasResized;
}

void* CSDL2WindowAdapter::getHandle()
{
    return pWindow;
}

const std::vector<SDL_Event>& CSDL2WindowAdapter::getWinEvents()
{
    return vWinEvents;
}