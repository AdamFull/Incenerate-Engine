#pragma once

#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace graphics
	{
		class CQueryPool
		{
		public:
			CQueryPool(CDevice* device);
			~CQueryPool();

			void create();
			void clear(vk::CommandBuffer& commandBuffer);

			void begin(vk::CommandBuffer& commandBuffer, uint32_t index);
			void end(vk::CommandBuffer& commandBuffer, uint32_t index);

			void takeResult();

			bool wasDrawn(uint32_t index);
		private:
			CDevice* pDevice{ nullptr };
			vk::QueryPool queryPool;
			std::map<uint32_t, uint32_t> mAssociation;
			std::vector<uint64_t> vPixelDrawn;
		};
	}
}