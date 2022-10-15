#pragma once

#include <memory>
#include <array>
#include <glm/glm/glm.hpp>


//based on https://gist.github.com/Eisenwave/c48bf988fc29d1c8bf0d4512d3916d22
namespace engine
{
	struct FOctreeNode
	{
		virtual ~FOctreeNode() = default;
	};

	struct FOctreeBranch : public FOctreeNode
	{
		std::array<std::unique_ptr<FOctreeNode>, 8> children;
		virtual ~FOctreeBranch() = default;
	};

	struct FOctreeLeaf : public FOctreeNode
	{
		std::array<uint32_t, 8> data{};
		virtual ~FOctreeLeaf() = default;
	};

	using octreevec_t = glm::i32vec3;
	using octreevecu_t = glm::u32vec3;

	class CSparseVoxelOctree
	{
		
	public:
		CSparseVoxelOctree();

		void insert(const octreevec_t& pos, uint32_t color);

		octreevec_t minIncl() const;
		octreevec_t maxIncl() const;
		octreevec_t minExcl() const;
		octreevec_t maxExcl() const;

		uint32_t& operator[](const octreevec_t& pos);
		uint32_t& at(const octreevec_t& pos);
		const uint32_t& at(const octreevec_t& pos) const;
	private:
		uint32_t& findOrCreate(uint64_t octreeNodeIndex);
		uint32_t* find(uint64_t octreeNodeIndex) const;

		uint64_t indexOf(const octreevec_t& pos) const;

		void ensureSpace(const octreevec_t& pos);
		void insert(uint64_t octreeNodeIndex, uint32_t color);

		void grow(uint32_t lim);
		void growOnce();

		uint32_t boundsTest(const octreevec_t& v) const;

	private:
		size_t depth = 1;
		std::unique_ptr<FOctreeBranch> root;
	};
}