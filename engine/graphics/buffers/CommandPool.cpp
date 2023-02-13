#include "CommandPool.h"

#include "APIHandle.h"

using namespace engine::graphics;

CCommandPool::CCommandPool(CDevice* device)
{
    pDevice = device;
}

void CCommandPool::create(vk::QueueFlags queueFlags, const std::thread::id& threadId)
{
    this->threadId = threadId;
    //TODO: refactor here device calls
    FQueueFamilyIndices queueFamilyIndices = pDevice->findQueueFamilies();

    vk::CommandPoolCreateInfo poolInfo = {};
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

    if(queueFlags & vk::QueueFlagBits::eGraphics)
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    else if(queueFlags & vk::QueueFlagBits::eCompute)
        poolInfo.queueFamilyIndex = queueFamilyIndices.computeFamily.value();
    else if((queueFlags & vk::QueueFlagBits::eTransfer) || (queueFlags & vk::QueueFlagBits::eSparseBinding))
        poolInfo.queueFamilyIndex = queueFamilyIndices.transferFamily.value();

    vk::Result res = pDevice->create(poolInfo, &commandPool);
    log_cerror(VkHelper::check(res), "Cannot create command pool.");
}

CCommandPool::~CCommandPool()
{
    pDevice->destroy(&commandPool);
    pDevice = nullptr;
}