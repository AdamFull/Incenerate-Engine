#pragma once

#include "Buffer.h"
#include "APIHandle.h"

namespace engine
{
	namespace graphics
	{
		template<class _Ty>
		class CAtomicVariable
		{
		public:
			CAtomicVariable(CDevice* device)
			{
				pDevice = device;
			}

			void create()
			{
				pBuffer = CBuffer::MakeBuffer(pDevice, sizeof(_Ty), 1, vma::MemoryUsage::eGpuOnly,
					vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst |
					vk::BufferUsageFlagBits::eTransferSrc);

				pStagingBuffer = CBuffer::MakeBuffer(pDevice, sizeof(_Ty), 1, vma::MemoryUsage::eCpuCopy,
					vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc);
			}

			void reset(const _Ty& value) 
			{
				pStagingBuffer->map();
				pStagingBuffer->write((void*)(&value));
				pStagingBuffer->unmap();
				pDevice->copyOnDeviceBuffer(pStagingBuffer->getBuffer(), pBuffer->getBuffer(), pBuffer->getBufferSize());
			}

			_Ty read() const
			{
				pDevice->copyOnDeviceBuffer(pBuffer->getBuffer(), pStagingBuffer->getBuffer(), pStagingBuffer->getBufferSize());
				pStagingBuffer->map();
				_Ty result = *((_Ty*)pStagingBuffer->getMappedMemory());
				pStagingBuffer->unmap();
				return result;
			}

		private:
			CDevice* pDevice{ nullptr };
			std::unique_ptr<CBuffer> pBuffer;
			std::unique_ptr<CBuffer> pStagingBuffer;
		};
	}
}