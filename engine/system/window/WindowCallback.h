#pragma once

#include <utility/ufunction.hpp>

#include <vector>

#include "input/KeycodeHelper.hpp"

namespace engine
{
	namespace system
	{
		namespace window
		{
            struct FWindowCallback
            {
                static void OnWindowSizeChanged(int32_t width, int32_t height);
                static void OnKeyStateChanges(input::EKeyCode keycode, input::EKeyState state);
                static void OnCursorMove(float dx, float dy);
                static void OnCursorInput(float x, float y);
                static void OnMouseWheel(float x, float y);

                template <class... Args>
                static void SubscribeSizeChange(Args... args)
                {
                    sizeChangeCallbacks.emplace_back(utl::function<void(int32_t, int32_t)>(std::forward<Args>(args)...));
                }

                template <class... Args>
                static void SubscribeKeyStateChange(Args... args)
                {
                    keyStateCallbacks.emplace_back(utl::function<void(input::EKeyCode, input::EKeyState)>(std::forward<Args>(args)...));
                }

                template <class... Args>
                static void SubscribeCursorMove(Args... args)
                {
                    cursorMoveCallbacks.emplace_back(utl::function<void(float, float)>(std::forward<Args>(args)...));
                }

                template <class... Args>
                static void SubscribeCursorInput(Args... args)
                {
                    cursorInputCallbacks.emplace_back(utl::function<void(float, float)>(std::forward<Args>(args)...));
                }

                template <class... Args>
                static void SubscribeScrollInput(Args... args)
                {
                    scrollCallbacks.emplace_back(utl::function<void(float, float)>(std::forward<Args>(args)...));
                }

            private:
                static std::vector<utl::function<void(int32_t, int32_t)>> sizeChangeCallbacks;
                static std::vector<utl::function<void(input::EKeyCode, input::EKeyState)>> keyStateCallbacks;
                static std::vector<utl::function<void(float, float)>> cursorMoveCallbacks;
                static std::vector<utl::function<void(float, float)>> cursorInputCallbacks;
                static std::vector<utl::function<void(float, float)>> scrollCallbacks;
            };
		}
	}
}
