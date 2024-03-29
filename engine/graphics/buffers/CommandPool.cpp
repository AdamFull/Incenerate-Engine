#include "CommandPool.h"

#include "APIHandle.h"
#include "APICompatibility.h"

using namespace engine::graphics;

CCommandPool::CCommandPool(CDevice* device)
{
    pDevice = device;
}

void CCommandPool::create(vk::QueueFlags queueFlags, const std::thread::id& threadId)
{
    this->threadId = threadId;

    vk::CommandPoolCreateInfo poolInfo = {};
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

    if((queueFlags & vk::QueueFlagBits::eGraphics) || (queueFlags & vk::QueueFlagBits::eCompute) || (queueFlags & vk::QueueFlagBits::eTransfer))
        poolInfo.queueFamilyIndex = pDevice->getQueueFamilyIndex(family::graphics);
    else if(queueFlags & vk::QueueFlagBits::eCompute)
        poolInfo.queueFamilyIndex = pDevice->getQueueFamilyIndex(family::compute);
    else if((queueFlags & vk::QueueFlagBits::eTransfer) || (queueFlags & vk::QueueFlagBits::eSparseBinding))
        poolInfo.queueFamilyIndex = pDevice->getQueueFamilyIndex(family::transfer);

    vk::Result res = pDevice->create(poolInfo, &commandPool);
    log_cerror(APICompatibility::check(res), "Cannot create command pool.");
}

CCommandPool::~CCommandPool()
{
    pDevice->destroy(&commandPool);
    pDevice = nullptr;
}

CDevice* CCommandPool::getDevice()
{
    return pDevice;
}