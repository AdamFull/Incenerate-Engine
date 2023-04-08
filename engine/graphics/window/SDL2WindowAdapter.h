#pragma once

#include "WindowAdapter.h"

#include <SDL.h>

namespace engine
{
	namespace system { namespace window { class CWindowHandle; } }

	namespace graphics
	{
		class CSDL2WindowAdapter : public IWindowAdapter
		{
		public:
			~CSDL2WindowAdapter() override;

			void create(const FWindowCreateInfo& createInfo) override;
			void destroy() override;
			bool processEvents() override;

			void createSurface(const vk::Instance& instance, vk::SurfaceKHR& surface, vk::AllocationCallbacks* allocator) override;

			std::vector<const char*> getWindowExtensions(bool validation) override;

			bool isMinimized() override;
			bool wasResized() override;

			void* getHandle() override;

			const std::vector<SDL_Event>& getWinEvents();
		private:
			SDL_Window* pWindow{ nullptr };
			std::vector<SDL_Event> vWinEvents;
		};
	}
}