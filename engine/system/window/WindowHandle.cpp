#include "WindowHandle.h"

#include <utility/logger/logger.h>
#include "Engine.h"

using namespace engine::system::window;
using namespace engine::ecs;

CWindowHandle::~CWindowHandle()
{
    destroy();
}

void CWindowHandle::create(const FWindowCreateInfo& createInfo)
{
    uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;

    iWidth = createInfo.width;
    iHeight = createInfo.height;
    
    SDL_Init(SDL_INIT_VIDEO);
    pWindow = SDL_CreateWindow(createInfo.srName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, iWidth, iHeight, flags);
    log_cerror((pWindow != nullptr), "Could not create window.");
    log_info("Created window: [name: {}, extent: {}x{}, aspect: {}]", createInfo.srName, iWidth, iHeight, getAspect());
}

void CWindowHandle::destroy()
{
    if (pWindow)
    {
        SDL_DestroyWindow(pWindow);
        pWindow = nullptr;
        SDL_Quit();
        log_debug("Window destroyed.");
    }
}

bool CWindowHandle::begin()
{
    bool bKeyStateChange{ false };

    if(!isMinimized())
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
            bKeyStateChange |= mKeys.update(event.key);
        } break;

        case SDL_TEXTINPUT:
        {
        } break;

        case SDL_CONTROLLERBUTTONUP:
        case SDL_CONTROLLERBUTTONDOWN:
        {
            bKeyStateChange |= mKeys.update(event.cbutton);
        } break;

        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
        {
            bKeyStateChange |= mKeys.update(event.button);
        };

        case SDL_MOUSEWHEEL:
        {
            //FWindowCallback::OnMouseWheel(event.wheel.x, event.wheel.y);
        } break;

        case SDL_FINGERMOTION:
        {
            //FWindowCallback::OnCursorMove(event.tfinger.dx, event.tfinger.dy);
        } break;

        case SDL_CONTROLLERTOUCHPADMOTION:
        {
            static float controller_xold{ 0.f }, controller_yold{ 0.f };

            //FWindowCallback::OnCursorMove(controller_xold - static_cast<float>(event.ctouchpad.x), controller_yold - static_cast<float>(event.ctouchpad.y));

            controller_xold = event.ctouchpad.x;
            controller_yold = event.ctouchpad.y;

        } break;

        case SDL_MOUSEMOTION:
        {
            CEvent eevent(Events::Input::Mouse);
            eevent.setParam(Events::Input::MouseX, static_cast<float>(event.motion.x));
            eevent.setParam(Events::Input::MouseY, static_cast<float>(event.motion.y));
            EGEngine->sendEvent(eevent);
        } break;

        case SDL_FINGERDOWN:
        case SDL_FINGERUP:
        {
            //FWindowCallback::OnCursorInput(event.tfinger.x, event.tfinger.y);
        } break;

        case SDL_CONTROLLERTOUCHPADDOWN:
        case SDL_CONTROLLERTOUCHPADUP:
        {
            //FWindowCallback::OnCursorInput(event.ctouchpad.x, event.ctouchpad.y);
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
        CEvent eevent(Events::Input::Key);
        eevent.setParam(Events::Input::Key, mKeys);
        EGEngine->sendEvent(eevent);
    }

    return bRunning;
}

void CWindowHandle::createContext(void*& pContext)
{
    pContext = SDL_GL_CreateContext(pWindow);
    SDL_GL_MakeCurrent(pWindow, pContext);
}

const std::vector<SDL_Event>& CWindowHandle::getWinEvents()
{
    return vWinEvents;
}

SDL_Window* CWindowHandle::getWindowPointer()
{
    return pWindow;
}

bool CWindowHandle::isMinimized()
{
    auto flags = SDL_GetWindowFlags(pWindow);
    return flags & SDL_WINDOW_MINIMIZED;
}

bool CWindowHandle::wasResized()
{
    return bWasResized;
}

void CWindowHandle::getWindowSize(int32_t* width, int32_t* height)
{
    *width = iWidth;
    *height = iHeight;
}

float CWindowHandle::getAspect()
{
    return static_cast<float>(iWidth) / static_cast<float>(iHeight);
}

int32_t CWindowHandle::getWidth()
{
    return iWidth;
}

int32_t CWindowHandle::getHeight()
{
    return iHeight;
}