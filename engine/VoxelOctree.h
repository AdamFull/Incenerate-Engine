#pragma once

#include <glm/glm/glm.hpp>
#include <vector>

namespace engine
{
	constexpr uint32_t kOctreeNodeNumMin = 1000000;
	constexpr uint32_t kOctreeNodeNumMax = 500000000;

	class CVoxelOctree
	{
	public:
		void push(const glm::u32vec3& pos, const glm::vec4& color);
		void push(const glm::u32vec3& pos, const glm::u8vec4& color);
		void push(const glm::u32vec3& pos, uint32_t color);

		void build();
	private:
		std::vector<glm::u32vec2> vVoxData;
		std::vector<uint32_t> vOctree;
		uint32_t level {12};
	};
}