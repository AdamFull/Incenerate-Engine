#pragma once

#include <vulkan/vulkan.hpp>

#include <thread>

namespace engine
{
	namespace graphics
	{
        class CDevice;

        class CCommandPool
        {
        public:
            CCommandPool(CDevice* device);
            ~CCommandPool();

            void create(const std::thread::id& threadId = std::this_thread::get_id());

            operator const vk::CommandPool& () const { return commandPool; }

            const vk::CommandPool& getCommandPool() const { return commandPool; }
            const std::thread::id& getThreadId() const { return threadId; }
        private:
            CDevice* pDevice;
            vk::CommandPool commandPool{ VK_NULL_HANDLE };
            std::thread::id threadId;
        };
	}
}