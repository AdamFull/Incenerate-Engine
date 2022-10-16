#include "Buffer.h"

#include "Device.h"

#include <array>

using namespace engine::graphics;

std::unique_ptr<CBuffer> CBuffer::MakeStagingBuffer(CDevice* device, size_t size, size_t count)
{
    auto buffer = std::make_unique<CBuffer>(device);
    buffer->create(size, count, VMA_MEMORY_USAGE_CPU_ONLY, vk::BufferUsageFlagBits::eTransferSrc);
    return buffer;
}

std::unique_ptr<CBuffer> CBuffer::MakeVertexBuffer(CDevice* device, size_t size, size_t count)
{
    auto buffer = std::make_unique<CBuffer>(device);
    buffer->create(size, count, VMA_MEMORY_USAGE_UNKNOWN, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, 1);
    return buffer;
}

std::unique_ptr<CBuffer> CBuffer::MakeIndexBuffer(CDevice* device, size_t size, size_t count)
{
    auto buffer = std::make_unique<CBuffer>(device);
    buffer->create(size, count, VMA_MEMORY_USAGE_UNKNOWN, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, 1);
    return buffer;
}

std::unique_ptr<CBuffer> CBuffer::MakeStorageBuffer(CDevice* device, size_t size, size_t count)
{
    auto physProps = device->getPhysical().getProperties();
    auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);
    
    auto buffer = std::make_unique<CBuffer>(device);
    buffer->create(size, count, VMA_MEMORY_USAGE_CPU_TO_GPU,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eStorageBuffer,
        minOffsetAllignment);

    return buffer;
}

std::unique_ptr<CBuffer> CBuffer::MakeUniformBuffer(CDevice* device, size_t size, size_t count)
{
    auto physProps = device->getPhysical().getProperties();
    auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);

    auto buffer = std::make_unique<CBuffer>(device);
    buffer->create(size, count, VMA_MEMORY_USAGE_CPU_TO_GPU, vk::BufferUsageFlagBits::eUniformBuffer, minOffsetAllignment);

    return buffer;
}

CBuffer::CBuffer(CDevice* device)
{
	pDevice = device;
}

CBuffer::~CBuffer()
{
    if (mappedMemory)
        unmap();
    if (buffer)
        vmaDestroyBuffer(pDevice->getVMAAllocator(), buffer, allocation);

    pDevice = nullptr;
}

void CBuffer::create(vk::DeviceSize instanceSize, vk::DeviceSize instanceCount, VmaMemoryUsage memory_usage, vk::BufferUsageFlags usageFlags, vk::DeviceSize minOffsetAlignment)
{
    auto logical = pDevice->getLogical();
    alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
    bufferSize = alignmentSize * instanceCount;

    auto queueFamilies = pDevice->findQueueFamilies();

    std::array queueFamily = { *queueFamilies.graphicsFamily, *queueFamilies.presentFamily, *queueFamilies.computeFamily }; //Make it set

    vk::BufferCreateInfo bufferInfo = {};
    bufferInfo.size = bufferSize;
    bufferInfo.usage = usageFlags;
    if(queueFamily.size() <= 1)
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;
    else
    {
        bufferInfo.sharingMode = vk::SharingMode::eExclusive; //TODO: make it concurent
        bufferInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamily.size());
        bufferInfo.pQueueFamilyIndices = queueFamily.data();
    }

    VmaAllocationCreateInfo alloc_create_info = {};
    alloc_create_info.usage = memory_usage;

    VkBufferCreateInfo bufferCI = (VkBufferCreateInfo)bufferInfo;
    VkBuffer tmpbuf;
    vmaCreateBuffer(pDevice->getVMAAllocator(), &bufferCI, &alloc_create_info, &tmpbuf, &allocation, nullptr);
    buffer = tmpbuf;
}

void CBuffer::reCreate(vk::DeviceSize instanceSize, uint32_t instanceCount, VmaMemoryUsage memory_usage, vk::BufferUsageFlags usageFlags,
     vk::DeviceSize minOffsetAlignment)
{
    create(instanceSize, instanceCount, memory_usage, usageFlags, minOffsetAlignment);
}

vk::DescriptorBufferInfo& CBuffer::getDescriptor(vk::DeviceSize size, vk::DeviceSize offset)
{
    bufferInfo.buffer = buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = size;

    if (offset != 0) {
        bufferInfo.offset = offset;
        bufferInfo.range = size;
    }

    return bufferInfo;
}

vk::DescriptorBufferInfo& CBuffer::getDescriptor()
{
    return getDescriptor(bufferSize, 0);
}

void CBuffer::map()
{
    auto& vkDevice = pDevice->getLogical();
    assert(vkDevice && buffer  && "Called map on buffer before create");

    auto res = vmaMapMemory(pDevice->getVMAAllocator(), allocation, &mappedMemory);
    assert(res == VK_SUCCESS && "Can't map memory.");
}

void CBuffer::unmap()
{
    auto& vkDevice = pDevice->getLogical();
    assert(vkDevice && "Called unmap buffer but device is invalid");
    if (mappedMemory)
        vmaUnmapMemory(pDevice->getVMAAllocator(), allocation);
}

bool CBuffer::compare(void* idata, vk::DeviceSize size, vk::DeviceSize offset)
{
    assert(mappedMemory && "Cannot compare to unmapped buffer");
    if (offset == 0)
    {
        return std::memcmp(mappedMemory, idata, size) != 0;
    }
    else
    {
        char* memOffset = (char*)mappedMemory;
        memOffset += offset;
        return std::memcmp(memOffset, idata, size) != 0;
    }
}

void CBuffer::write(void* idata, vk::DeviceSize size, vk::DeviceSize offset)
{
    assert(mappedMemory && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE)
    {
        std::memcpy(mappedMemory, idata, bufferSize);
    }
    else
    {
        char* memOffset = (char*)mappedMemory;
        memOffset += offset;
        std::memcpy(memOffset, idata, size);
    }
}

void CBuffer::flush(vk::DeviceSize size, vk::DeviceSize offset)
{
    auto& vkDevice = pDevice->getLogical();
    assert(vkDevice && "Called flush buffer but device is invalid");
    vmaFlushAllocation(pDevice->getVMAAllocator(), allocation, offset, size);
}

vk::DeviceSize CBuffer::getAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignment)
{
    if (minOffsetAlignment > 0)
    {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}