#pragma once

#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace graphics
	{
        class CCommandBuffer
        {
        public:
            CCommandBuffer() = default;
            CCommandBuffer(CDevice* device);
            ~CCommandBuffer();

            void create(bool _begin, vk::QueueFlags queueFlags, vk::CommandBufferLevel bufferLevel = vk::CommandBufferLevel::ePrimary, uint32_t count = 1);
            void begin(vk::CommandBufferUsageFlags usage = vk::CommandBufferUsageFlagBits::eOneTimeSubmit, uint32_t index = 0);
            void end();

            vk::Result submitIdle();

            vk::Result submit(uint32_t& imageIndex);

            operator const vk::CommandBuffer& () const { return vCommandBuffers.at(frameIndex); }

            vk::CommandBuffer& getCommandBuffer() { return vCommandBuffers.at(frameIndex); }
            bool isRunning() const { return running; }

            CDevice* getDevice();

        private:
            CDevice* pDevice{ nullptr };

            vk::Queue getQueue() const;
            uint32_t frameIndex{ 0 };

            std::shared_ptr<CCommandPool> commandPool;

            vk::QueueFlags queueFlags;
            std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> vCommandBuffers;
            bool running = false;
        };
	}
}