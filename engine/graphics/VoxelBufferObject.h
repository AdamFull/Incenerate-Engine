#pragma once

#include <glm/glm/glm.hpp>
#include "buffers/AtomicVariable.h"

namespace engine
{
	namespace graphics
	{
		constexpr uint32_t kOctreeNodeNumMin = 1000000;
		constexpr uint32_t kOctreeNodeNumMax = 500000000;

		class CVoxelBufferObject
		{
		public:
			CVoxelBufferObject(CDevice* device);
			void create();
			void push(const glm::u32vec3& pos, const glm::vec4& color);
			void push(const glm::u32vec3& pos, const glm::u8vec4& color);
			void push(const glm::u32vec3& pos, uint32_t color);

			void build();
		private:
			CDevice* pDevice;

			std::vector<glm::u32vec2> vVoxData;
			std::vector<uint32_t> vOctree;
			uint32_t level{ 12 };

			std::unique_ptr<CAtomicVariable<uint32_t>> pCounter;
			std::unique_ptr<CBuffer> pBuildInfoBuffer;
			std::unique_ptr<CBuffer> pIndirectBuffer;
			std::unique_ptr<CBuffer> pOctreeBuffer;
		};
	}
}