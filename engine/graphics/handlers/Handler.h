#pragma once

#include "shader/Shader.h"
#include "buffers/Buffer.h"

namespace engine
{
	namespace graphics
	{
		enum class EHandlerStatus
		{
			eNone,
			eChanged,
			eFlushed
		};

		class CHandler
		{
		public:
			CHandler() = default;
			CHandler(CDevice* device);
			virtual ~CHandler() = default;

			virtual void create(const CUniformBlock& uniformBlock);
			void reCreate();
			void flush();

			const std::unique_ptr<CBuffer>& getBuffer();

			uint32_t getCurrentFrameProxy();

			template<class _Ty>
			void write(_Ty& object)
			{
				if (!m_uniformBlock || vBuffers.empty())
					return;

				auto index = getCurrentFrameProxy();
				auto& pBuffer = vBuffers.at(index);

				if (!vMapped.at(index))
				{
					pBuffer->map();
					vMapped.at(index) = true;
				}

				auto size = sizeof(object);
				//if (status == EHandlerStatus::eChanged || pBuffer->compare((void*)&object, size, 0))
				//{
					pBuffer->write((void*)&object, size, 0);
					status = EHandlerStatus::eChanged;
				//}

				flush();
			}

			template<class _Ty>
			void set(_Ty& object, std::size_t offset, std::size_t size)
			{
				if (!m_uniformBlock || vBuffers.empty())
					return;

				auto index = getCurrentFrameProxy();
				auto& pBuffer = vBuffers.at(index);

				if (!vMapped.at(index))
				{
					pBuffer->map();
					vMapped.at(index) = true;
				}

				//if (status == EHandlerStatus::eChanged || pBuffer->compare((void*)&object, size, offset))
				//{
					pBuffer->write((void*)&object, size, offset);
					status = EHandlerStatus::eChanged;
				//}
			}

			template<class _Ty>
			void set(const std::string& uniformName, const _Ty& object, std::size_t size = 0)
			{
				if (!m_uniformBlock)
					return;

				auto uniform = m_uniformBlock->getUniform(uniformName);
				if (!uniform)
					return;

				auto realSize = size;
				if (realSize == 0)
					realSize = std::min(sizeof(object), uniform->getSize());

				set(object, uniform->getOffset(), realSize);
			}

			CDevice* getDevice();

		protected:
			CDevice* pDevice{ nullptr };
			std::optional<CUniformBlock> m_uniformBlock;
			std::vector<std::unique_ptr<CBuffer>> vBuffers;
			std::vector<bool> vMapped;
			EHandlerStatus status{ EHandlerStatus::eNone };
		};
	}
}