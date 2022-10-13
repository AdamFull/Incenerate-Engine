#include "CommandPool.h"

#include "Device.h"

using namespace engine::graphics;

CCommandPool::CCommandPool(CDevice* device)
{
    pDevice = device;
}

void CCommandPool::create(const std::thread::id& threadId)
{
    this->threadId = threadId;
    //TODO: refactor here device calls
    FQueueFamilyIndices queueFamilyIndices = pDevice->findQueueFamilies();

    vk::CommandPoolCreateInfo poolInfo = {};
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    vk::Result res = pDevice->create(poolInfo, &commandPool);
    assert(res == vk::Result::eSuccess && "Cannot create command pool.");
}

CCommandPool::~CCommandPool()
{
    pDevice->destroy(&commandPool);
    pDevice = nullptr;
}