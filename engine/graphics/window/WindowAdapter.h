#pragma once

#include "EngineStructures.h"
#include "Keycode.h"

#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace graphics
	{
		class IWindowAdapter
		{
		public:
			virtual ~IWindowAdapter() = default;

			virtual void create(const FWindowCreateInfo& createInfo) = 0;
			virtual void destroy() = 0;
			virtual bool processEvents() = 0;

			virtual void createSurface(const vk::Instance& instance, vk::SurfaceKHR& surface, vk::AllocationCallbacks* allocator) = 0;

			virtual std::vector<const char*> getWindowExtensions(bool validation) = 0;

			virtual bool isMinimized() = 0;
			virtual bool wasResized() = 0;

			virtual void getWindowSize(int32_t* width, int32_t* height);
			virtual float getAspect();
			virtual int32_t getWidth();
			virtual int32_t getHeight();

			virtual void* getHandle() = 0;
		protected:
			CKeyDecoder mKeys;
			int32_t iWidth{ 0 }, iHeight{ 0 };
			bool bWasResized{ false };
			bool bRunning{ true };
		};
	}
}