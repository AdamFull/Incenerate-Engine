#pragma once

#include "shader/Shader.h"
#include <optional>

namespace engine
{
	namespace graphics
	{
		class CPushHandler
		{
		public:
			CPushHandler() = default;
			CPushHandler(CPipeline* pipe);
			~CPushHandler();

			void create(const CPushConstBlock& pushBlock);
			void flush(vk::CommandBuffer& commandBuffer);

			template<class T>
			void set(const T& object, std::size_t offset, std::size_t size)
			{
				auto index = getCurrentFrameProxy();
				std::memcpy(vData.at(index).get() + offset, &object, size);
			}

			template<class T>
			void set(const std::string& uniformName, const T& object, std::size_t size = 0)
			{
				if (!pushBlock)
					return;

				auto uniform = pushBlock->getUniform(uniformName);
				if (!uniform)
					return;

				auto realSize = size;
				if (realSize == 0)
					realSize = std::min(sizeof(object), uniform->getSize());

				set(object, uniform->getOffset(), realSize);
			}

		private:
			uint32_t getCurrentFrameProxy();
		private:
			CPipeline* pPipeline{ nullptr };

			std::optional<CPushConstBlock> pushBlock;
			std::vector<std::unique_ptr<char[]>> vData;
		};
	}
}