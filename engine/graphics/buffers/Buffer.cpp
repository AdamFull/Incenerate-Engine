#include "Buffer.h"

#include "Device.h"

#include <array>

using namespace engine::graphics;

CBuffer::CBuffer(CDevice* device)
{
	pDevice = device;
}

CBuffer::~CBuffer()
{
    if (mappedMemory)
        unmap();
    if (buffer)
        pDevice->destroy(&buffer);
    if (deviceMemory)
        pDevice->destroy(&deviceMemory);

    pDevice = nullptr;
}

void CBuffer::create(vk::DeviceSize instanceSize, vk::DeviceSize instanceCount, vk::BufferUsageFlags usageFlags, 
    vk::MemoryPropertyFlags memoryPropertyFlags, vk::DeviceSize minOffsetAlignment)
{
    auto logical = pDevice->getLogical();
    alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
    bufferSize = alignmentSize * instanceCount;

    auto queueFamilies = pDevice->findQueueFamilies();

    std::array queueFamily = { *queueFamilies.graphicsFamily, *queueFamilies.presentFamily, *queueFamilies.computeFamily };

    vk::BufferCreateInfo bufferInfo = {};
    bufferInfo.size = bufferSize;
    bufferInfo.usage = usageFlags;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;
    bufferInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamily.size());
    bufferInfo.pQueueFamilyIndices = queueFamily.data();

    vk::Result res = pDevice->create(bufferInfo, &buffer);
    assert(res == vk::Result::eSuccess && "On device buffer was not created");

    vk::MemoryRequirements memRequirements = logical.getBufferMemoryRequirements(buffer);
    this->memoryPropertyFlags = memoryPropertyFlags;

    vk::MemoryAllocateInfo allocInfo = {};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = pDevice->findMemoryType(memRequirements.memoryTypeBits, memoryPropertyFlags);

    res = pDevice->create(allocInfo, &deviceMemory);
    assert(res == vk::Result::eSuccess && "Buffer memory was not allocated");

    logical.bindBufferMemory(buffer, deviceMemory, 0);
}

void CBuffer::reCreate(vk::DeviceSize instanceSize, uint32_t instanceCount, vk::BufferUsageFlags usageFlags, 
    vk::MemoryPropertyFlags memoryPropertyFlags, vk::DeviceSize minOffsetAlignment)
{
    create(instanceSize, instanceCount, usageFlags, memoryPropertyFlags, minOffsetAlignment);
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

void CBuffer::map(vk::DeviceSize size, vk::DeviceSize offset)
{
    auto& vkDevice = pDevice->getLogical();
    assert(vkDevice && buffer && deviceMemory && "Called map on buffer before create");
    if (size == VK_WHOLE_SIZE)
    {
        auto res = vkDevice.mapMemory(deviceMemory, 0, bufferSize, vk::MemoryMapFlags{}, &mappedMemory);
        assert(res == vk::Result::eSuccess && "Can't map memory.");
        return;
    }
    auto res = vkDevice.mapMemory(deviceMemory, offset, size, vk::MemoryMapFlags{}, &mappedMemory);
    assert(res == vk::Result::eSuccess && "Can't map memory.");
}

void CBuffer::unmap()
{
    auto& vkDevice = pDevice->getLogical();
    assert(vkDevice && "Called unmap buffer but device is invalid");
    if (mappedMemory)
        vkDevice.unmapMemory(deviceMemory);
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

vk::Result CBuffer::flush(vk::DeviceSize size, vk::DeviceSize offset)
{
    auto& vkDevice = pDevice->getLogical();
    assert(vkDevice && "Called flush buffer but device is invalid");
    vk::MappedMemoryRange mappedRange = {};
    mappedRange.memory = deviceMemory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkDevice.flushMappedMemoryRanges(1, &mappedRange);
}

vk::DeviceSize CBuffer::getAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignment)
{
    if (minOffsetAlignment > 0)
    {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}