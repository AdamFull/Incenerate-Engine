#pragma once

namespace engine
{
	namespace graphics
	{
		class CBuffer
		{
		public:
			CBuffer(CDevice* device);
			~CBuffer();

			static std::unique_ptr<CBuffer> MakeBuffer(CDevice* device, size_t size, size_t count,
				VmaMemoryUsage memory_usage, vk::BufferUsageFlags usageFlags,
				vk::DeviceSize minOffsetAlignment = 1);

			static std::unique_ptr<CBuffer> MakeStagingBuffer(CDevice* device, size_t size, size_t count);
			static std::unique_ptr<CBuffer> MakeVertexBuffer(CDevice* device, size_t size, size_t count);
			static std::unique_ptr<CBuffer> MakeIndexBuffer(CDevice* device, size_t size, size_t count);
			static std::unique_ptr<CBuffer> MakeStorageBuffer(CDevice* device, size_t size, size_t count);
			static std::unique_ptr<CBuffer> MakeUniformBuffer(CDevice* device, size_t size, size_t count);

			void create(vk::DeviceSize instanceSize, vk::DeviceSize instanceCount, VmaMemoryUsage memory_usage, vk::BufferUsageFlags usageFlags,
				vk::DeviceSize minOffsetAlignment = 1);
			void reCreate(vk::DeviceSize instanceSize, uint32_t instanceCount, VmaMemoryUsage memory_usage, vk::BufferUsageFlags usageFlags,
				vk::DeviceSize minOffsetAlignment = 1);

			vk::DescriptorBufferInfo& getDescriptor(vk::DeviceSize size, vk::DeviceSize offset);
			vk::DescriptorBufferInfo& getDescriptor();

			void map();
			void unmap();

			bool compare(void* idata, vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
			void write(void* idata, vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
			void flush(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);

			vk::Buffer getBuffer() const { return buffer; }
			void* getMappedMemory() const { return mappedMemory; }
			uint32_t getInstanceCount() const { return instanceCount; }
			vk::DeviceSize getInstanceSize() const { return instanceSize; }
			vk::DeviceSize getAlignmentSize() const { return instanceSize; }
			vk::BufferUsageFlags getUsageFlags() const { return usageFlags; }
			vk::DeviceSize getBufferSize() const { return bufferSize; }

			static vk::DeviceSize getAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignment);

		protected:
			CDevice* pDevice{ nullptr };
			void* mappedMemory{ nullptr };
			vk::Buffer buffer = VK_NULL_HANDLE;
			VmaAllocation allocation{ VK_NULL_HANDLE };

			vk::DescriptorBufferInfo bufferInfo;
			vk::DeviceSize bufferSize;
			uint32_t instanceCount;
			vk::DeviceSize instanceSize{ 0 };
			vk::DeviceSize alignmentSize{ 0 };
			vk::BufferUsageFlags usageFlags;
		};
	}
}