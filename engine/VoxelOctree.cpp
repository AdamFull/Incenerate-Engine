#include "VoxelOctree.h"

#include <algorithm>

using namespace engine;

template<class _Ty>
uint32_t packUnorm4x8(const _Ty& color)
{
	return (color.w << 24u) | (color.z << 16u) | (color.y << 8u) | color.x;
}

void CVoxelOctree::push(const glm::u32vec3& pos, const glm::vec4& color)
{
	push(pos, packUnorm4x8(color));
}

void CVoxelOctree::push(const glm::u32vec3& pos, const glm::u8vec4& color)
{
	push(pos, packUnorm4x8(color));
}

void CVoxelOctree::push(const glm::u32vec3& pos, uint32_t color)
{
	glm::u32vec2 pack;
	pack.x = pos.x | (pos.y << 12u) | ((pos.z & 0xffu) << 24u);
	pack.y = ((pos.z >> 8u) << 28u) | (color & 0x00ffffffu);
	vVoxData.emplace_back(std::move(pack));
}

void CVoxelOctree::build()
{
	uint32_t octree_node_num = std::max(kOctreeNodeNumMin, static_cast<uint32_t>(vVoxData.size()) << 2u);
	octree_node_num = std::min(octree_node_num, kOctreeNodeNumMax);
	vOctree.reserve(octree_node_num);

	for (uint32_t i = 1; i < level; i++)
	{

	}
}