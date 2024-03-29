#include "CommandBuffer.h"

#include "APIHandle.h"
#include "APICompatibility.h"
#include "CommandPool.h"

using namespace engine::graphics;

CCommandBuffer::CCommandBuffer(CDevice* device)
{
    pDevice = device;
}

CCommandBuffer::~CCommandBuffer()
{
    auto& vkDevice = pDevice->getLogical();
    vkDevice.freeCommandBuffers(commandPool->getCommandPool(), vCommandBuffers);
    vCommandBuffers.clear();
    pDevice = nullptr;
}

void CCommandBuffer::create(bool _begin, vk::QueueFlags queueFlags, vk::CommandBufferLevel bufferLevel, uint32_t count)
{
    this->queueFlags = queueFlags;
    commandPool = pDevice->getCommandPool(queueFlags);

    vk::CommandBufferAllocateInfo allocInfo = {};
    allocInfo.commandPool = commandPool->getCommandPool();
    allocInfo.level = bufferLevel;
    allocInfo.commandBufferCount = count;

    vCommandBuffers.resize(count);
    vk::Result res = pDevice->create(allocInfo, vCommandBuffers.data());
    log_cerror(APICompatibility::check(res), "Cannot create bommand buffers");

    if (_begin)
        begin();
}

void CCommandBuffer::begin(vk::CommandBufferUsageFlags usage, uint32_t index)
{
    if (running)
        return;

    frameIndex = index;
    vk::CommandBufferBeginInfo beginInfo = {};
    beginInfo.flags = usage;
    auto commandBuffer = getCommandBuffer();
    commandBuffer.begin(beginInfo);
    running = true;
}

void CCommandBuffer::end()
{
    if (!running) return;

    auto commandBuffer = getCommandBuffer();
    commandBuffer.end();

    running = false;
}


vk::Result CCommandBuffer::submitIdle()
{
    auto& vkDevice = pDevice->getLogical();
    log_cerror(vkDevice, "Trying to submit queue, but device is invalid.");
    vk::Result res;
    if (running)
        end();

    vk::SubmitInfo submitInfo{};
    submitInfo.sType = vk::StructureType::eSubmitInfo;
    submitInfo.commandBufferCount = vCommandBuffers.size();
    submitInfo.pCommandBuffers = vCommandBuffers.data();

    vk::FenceCreateInfo fenceCreateInfo{};
    vk::Fence fence;
    res = pDevice->create(fenceCreateInfo, &fence);
    log_cerror(APICompatibility::check(res), "Failed to create fence. Reason: {}.", APICompatibility::get_error(res));
    res = vkDevice.resetFences(1, &fence);
    log_cerror(APICompatibility::check(res), "Failed to reset fence. Reason: {}.", APICompatibility::get_error(res));

    auto& queue = pDevice->getQueue(queueFlags);
    queue.submit(submitInfo, fence);
    res = vkDevice.waitForFences(1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
    log_cerror(APICompatibility::check(res), "Failed to wait for fences. Reason: {}.", APICompatibility::get_error(res));
    pDevice->destroy(&fence);
    return res;
}

vk::Result CCommandBuffer::submit(uint32_t& imageIndex)
{
    if (running)
        end();

    auto commandBuffer = getCommandBuffer();
    return pDevice->submitCommandBuffers(&commandBuffer, &imageIndex, queueFlags);
}

CDevice* CCommandBuffer::getDevice()
{
    return pDevice;
}