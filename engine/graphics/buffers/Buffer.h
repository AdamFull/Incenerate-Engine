#pragma once

#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace graphics
	{
		class CDevice;

		class CBuffer
		{
		public:
			CBuffer(CDevice* device);
			~CBuffer();

			void create(vk::DeviceSize instanceSize, vk::DeviceSize instanceCount, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
				vk::DeviceSize minOffsetAlignment = 1);
			void reCreate(vk::DeviceSize instanceSize, uint32_t instanceCount, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
				vk::DeviceSize minOffsetAlignment = 1);

			vk::DescriptorBufferInfo& getDescriptor(vk::DeviceSize size, vk::DeviceSize offset);
			vk::DescriptorBufferInfo& getDescriptor();

			void map(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
			void unmap();

			bool compare(void* idata, vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
			void write(void* idata, vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
			vk::Result flush(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);

			vk::Buffer getBuffer() const { return buffer; }
			void* getMappedMemory() const { return mappedMemory; }
			uint32_t getInstanceCount() const { return instanceCount; }
			vk::DeviceSize getInstanceSize() const { return instanceSize; }
			vk::DeviceSize getAlignmentSize() const { return instanceSize; }
			vk::BufferUsageFlags getUsageFlags() const { return usageFlags; }
			vk::MemoryPropertyFlags getMemoryPropertyFlags() const { return memoryPropertyFlags; }
			vk::DeviceSize getBufferSize() const { return bufferSize; }

			static vk::DeviceSize getAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignment);

		protected:
			CDevice* pDevice{ nullptr };
			void* mappedMemory{ nullptr };
			vk::Buffer buffer = VK_NULL_HANDLE;
			vk::DeviceMemory deviceMemory = VK_NULL_HANDLE;

			vk::DescriptorBufferInfo bufferInfo;
			vk::DeviceSize bufferSize;
			uint32_t instanceCount;
			vk::DeviceSize instanceSize{ 0 };
			vk::DeviceSize alignmentSize{ 0 };
			vk::BufferUsageFlags usageFlags;
			vk::MemoryPropertyFlags memoryPropertyFlags;
		};
	}
}