#include "WindowCallback.h"

using namespace engine::system::input;
using namespace engine::system::window;

std::vector<utl::function<void(int32_t, int32_t)>> FWindowCallback::sizeChangeCallbacks;
std::vector<utl::function<void(EKeyCode, EKeyState)>> FWindowCallback::keyStateCallbacks;
std::vector<utl::function<void(float, float)>> FWindowCallback::cursorMoveCallbacks;
std::vector<utl::function<void(float, float)>> FWindowCallback::cursorInputCallbacks;
std::vector<utl::function<void(float, float)>> FWindowCallback::scrollCallbacks;

void FWindowCallback::OnWindowSizeChanged(int32_t width, int32_t height)
{
    for (auto& functor : sizeChangeCallbacks)
        functor(width, height);
}

void FWindowCallback::OnKeyStateChanges(EKeyCode keycode, EKeyState state)
{
    for (auto& functor : keyStateCallbacks)
        functor(keycode, state);
}

void FWindowCallback::OnCursorMove(float dx, float dy)
{
    for (auto& functor : cursorMoveCallbacks)
        functor(dx, dy);
}

void FWindowCallback::OnCursorInput(float x, float y)
{
    for (auto& functor : cursorMoveCallbacks)
        functor(x, y);
}

void FWindowCallback::OnMouseWheel(float x, float y)
{
    for (auto& functor : scrollCallbacks)
        functor(x, y);
}