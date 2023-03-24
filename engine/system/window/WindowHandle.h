#pragma once

#include <SDL.h>
#undef main

#include "EngineStructures.h"
#include "KeycodeHelper.hpp"

namespace engine
{
	namespace system
	{
		namespace window
		{
            class CWindowHandle
            {
            public:
                CWindowHandle() = default;
                ~CWindowHandle();

                void create(const FWindowCreateInfo& createInfo);
                void destroy();
                bool begin();
                void createContext(void*& pContext);

                const std::vector<SDL_Event>& getWinEvents();

                SDL_Window* getWindowPointer();
                
                bool isMinimized();
                bool wasResized();

                void getWindowSize(int32_t* width, int32_t* height);
                float getAspect();
                int32_t getWidth();
                int32_t getHeight();
            private:
                SDL_Window* pWindow{ nullptr };
                CKeyDecoder mKeys;
                std::vector<SDL_Event> vWinEvents;
                int32_t iWidth{ 0 }, iHeight{ 0 };
                bool bWasResized{ false };
                bool bRunning{ true };
            };
		}
	}
}