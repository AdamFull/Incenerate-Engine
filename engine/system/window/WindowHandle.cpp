#include "WindowHandle.h"
#include "WindowCallback.h"

using namespace engine::system::input;
using namespace engine::system::window;

int32_t CWindowHandle::iWidth{ 0 }, CWindowHandle::iHeight{ 0 };

CWindowHandle::~CWindowHandle()
{
    destroy();
}

void CWindowHandle::create(const FEngineCreateInfo& createInfo)
{
    uint32_t flags = SDL_WINDOW_SHOWN;

    switch (createInfo.eAPI)
    {
    case ERenderApi::eOpenGL: flags |= SDL_WINDOW_OPENGL; break;
    case ERenderApi::eVulkan: flags |= SDL_WINDOW_VULKAN; break;
    case ERenderApi::eOpenGLES: flags |= SDL_WINDOW_OPENGL; break;
    }

    iWidth = createInfo.window.width;
    iHeight = createInfo.window.height;
    
    SDL_Init(SDL_INIT_VIDEO);
    pWindow = SDL_CreateWindow(createInfo.window.srName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, iWidth, iHeight, flags);
}

void CWindowHandle::destroy()
{
    if (pWindow)
    {
        SDL_DestroyWindow(pWindow);
        pWindow = nullptr;
        SDL_Quit();
    }
}

bool CWindowHandle::begin()
{
    SDL_Event event;
    vWinEvents.clear();

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
            FWindowCallback::OnKeyStateChanges(FKeycodeHelper::KeyboardCodeToEngineCode(event.key.keysym.scancode), (EKeyState)event.key.state);
        } break;

        case SDL_TEXTINPUT:
        {
        } break;

        case SDL_CONTROLLERBUTTONUP:
        case SDL_CONTROLLERBUTTONDOWN:
        {
            FWindowCallback::OnKeyStateChanges(FKeycodeHelper::GamepadCodeToEngineCode((SDL_GameControllerButton)event.cbutton.button), (EKeyState)event.cbutton.state);
        } break;

        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
        {
            FWindowCallback::OnCursorInput(event.button.x, event.button.y);
            FWindowCallback::OnKeyStateChanges(FKeycodeHelper::MouseCodeToEngineCode(event.button.button), (EKeyState)event.button.state);
        };

        case SDL_MOUSEWHEEL:
        {
            FWindowCallback::OnMouseWheel(event.wheel.x, event.wheel.y);
        } break;

        case SDL_FINGERMOTION:
        {
            FWindowCallback::OnCursorMove(event.tfinger.dx, event.tfinger.dy);
        } break;

        case SDL_CONTROLLERTOUCHPADMOTION:
        {
            static float controller_xold{ 0.f }, controller_yold{ 0.f };

            FWindowCallback::OnCursorMove(controller_xold - static_cast<float>(event.ctouchpad.x), controller_yold - static_cast<float>(event.ctouchpad.y));

            controller_xold = event.ctouchpad.x;
            controller_yold = event.ctouchpad.y;

        } break;

        case SDL_MOUSEMOTION:
        {
            static float mouse_xold{ 0.f }, mouse_yold{ 0.f };

            FWindowCallback::OnCursorMove(mouse_xold - static_cast<float>(event.motion.x), mouse_yold - static_cast<float>(event.motion.y));

            mouse_xold = event.motion.x;
            mouse_yold = event.motion.y;
        } break;

        case SDL_FINGERDOWN:
        case SDL_FINGERUP:
        {
            FWindowCallback::OnCursorInput(event.tfinger.x, event.tfinger.y);
        } break;

        case SDL_CONTROLLERTOUCHPADDOWN:
        case SDL_CONTROLLERTOUCHPADUP:
        {
            FWindowCallback::OnCursorInput(event.ctouchpad.x, event.ctouchpad.y);
        } break;

        case SDL_CONTROLLERAXISMOTION:
        {
            event.caxis.type;
        } break;

        case SDL_WINDOWEVENT:
        {
            Uint8 window_event = event.window.event;


        } break;
        }

        vWinEvents.emplace_back(event);
    }

    return bRunning;
}

void CWindowHandle::end()
{
    SDL_GL_SwapWindow(pWindow);
}

const std::vector<SDL_Event>& CWindowHandle::getWinEvents()
{
    return vWinEvents;
}

void CWindowHandle::getWindowSize(int32_t* width, int32_t* height)
{
    *width = iWidth;
    *height = iHeight;
}

SDL_Window* CWindowHandle::getWindowPointer()
{
    return pWindow;
}

void CWindowHandle::createContext(void*& pContext)
{
    pContext = SDL_GL_CreateContext(pWindow);
    SDL_GL_MakeCurrent(pWindow, pContext);
}