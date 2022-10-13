#pragma once

#include <SDL.h>
#undef main

#include <vector>
#include "EngineStructures.h"

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

                void create(const FEngineCreateInfo& createInfo);
                void destroy();

                bool begin();
                void end();

                const std::vector<SDL_Event>& getWinEvents();

                void getWindowSize(int32_t* width, int32_t* height);

                SDL_Window* getWindowPointer();
                void createContext(void*& pContext);

                static int32_t iWidth, iHeight;

            private:
                SDL_Window* pWindow{ nullptr };
                std::vector<SDL_Event> vWinEvents;
                bool bRunning{ true };
            };
		}
	}
}