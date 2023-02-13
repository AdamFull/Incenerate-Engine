#pragma once

#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace graphics
	{
        class CCommandPool
        {
        public:
            CCommandPool(CDevice* device);
            ~CCommandPool();

            void create(vk::QueueFlags queueFlags, const std::thread::id& threadId = std::this_thread::get_id());

            operator const vk::CommandPool& () const { return commandPool; }

            const vk::CommandPool& getCommandPool() const { return commandPool; }
            const std::thread::id& getThreadId() const { return threadId; }
        private:
            CDevice* pDevice{ nullptr };
            vk::CommandPool commandPool{ VK_NULL_HANDLE };
            std::thread::id threadId;
        };
	}
}