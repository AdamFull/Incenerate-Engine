#include "VoxelBufferObject.h"

#include "Device.h"

#include <algorithm>

using namespace engine::graphics;

template<class _Ty>
uint32_t packUnorm4x8(const _Ty& color)
{
	return (color.w << 24u) | (color.z << 16u) | (color.y << 8u) | color.x;
}

CVoxelBufferObject::CVoxelBufferObject(CDevice* device)
{
	pDevice = device;
	vVoxData.reserve(kOctreeNodeNumMin);
}

void CVoxelBufferObject::create()
{
	pCounter = std::make_unique<CAtomicVariable<uint32_t>>(pDevice);
	pCounter->create();
	pCounter->reset(0u);

	pBuildInfoBuffer = CBuffer::MakeBuffer(pDevice, sizeof(uint32_t), 2, vma::MemoryUsage::eGpuOnly, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst);
	auto build_info_staging = CBuffer::MakeStagingBuffer(pDevice, sizeof(uint32_t), 2);

	{
		build_info_staging->map();
		auto* data = (uint32_t*)build_info_staging->getMappedMemory();
		data[0] = 0; // uAllocBegin
		data[1] = 8; // uAllocNum
		build_info_staging->unmap();
	}
	pDevice->copyOnDeviceBuffer(build_info_staging->getBuffer(), pBuildInfoBuffer->getBuffer(), pBuildInfoBuffer->getBufferSize());

	pIndirectBuffer = CBuffer::MakeBuffer(pDevice, sizeof(uint32_t), 3, vma::MemoryUsage::eGpuOnly, vk::BufferUsageFlagBits::eStorageBuffer |
		vk::BufferUsageFlagBits::eIndirectBuffer | vk::BufferUsageFlagBits::eTransferDst);
	auto indirect_staging = CBuffer::MakeStagingBuffer(pDevice, sizeof(uint32_t), 3);

	{
		indirect_staging->map();
		auto* data = (uint32_t*)indirect_staging->getMappedMemory();
		data[0] = 1; // uGroupX
		data[1] = 1; // uGroupY
		data[2] = 1; // uGroupZ
		indirect_staging->unmap();
	}
	pDevice->copyOnDeviceBuffer(indirect_staging->getBuffer(), pIndirectBuffer->getBuffer(), pIndirectBuffer->getBufferSize());

	uint32_t octree_node_num = std::max(kOctreeNodeNumMin, ((uint32_t)vVoxData.size()) << 2u);
	octree_node_num = std::min(octree_node_num, kOctreeNodeNumMax);

	pOctreeBuffer = CBuffer::MakeBuffer(pDevice, sizeof(uint32_t), octree_node_num, vma::MemoryUsage::eGpuOnly, vk::BufferUsageFlagBits::eStorageBuffer);
}

void CVoxelBufferObject::push(const glm::u32vec3& pos, const glm::vec4& color)
{
	push(pos, packUnorm4x8(color));
}

void CVoxelBufferObject::push(const glm::u32vec3& pos, const glm::u8vec4& color)
{
	push(pos, packUnorm4x8(color));
}

void CVoxelBufferObject::push(const glm::u32vec3& pos, uint32_t color)
{
	glm::u32vec2 pack;
	pack.x = pos.x | (pos.y << 12u) | ((pos.z & 0xffu) << 24u);
	pack.y = ((pos.z >> 8u) << 28u) | (color & 0x00ffffffu);
	vVoxData.emplace_back(std::move(pack));
}

void CVoxelBufferObject::build()
{
	
}