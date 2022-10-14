#pragma once

#include "Buffer.h"
#include "APIStructures.h"

#include <utility/upattern.hpp>

namespace engine
{
	namespace graphics
	{
		class CVertexBufferObject
		{
		public:
			CVertexBufferObject() = default;
			CVertexBufferObject(CDevice* device);
			~CVertexBufferObject() = default;

			void create();
			void bind(vk::CommandBuffer commandBuffer);
			void addVertices(std::vector<FVertex>&& vertices);
			void addIndices(std::vector<uint32_t>&& indices);
			void addMeshData(std::vector<FVertex>&& vertices, std::vector<uint32_t>&& indices);
			inline uint64_t getLastIndex();
			inline uint64_t getLastVertex();

		private:
			void createVertexBuffer();
			void createIndexBuffer();

		private:
			CDevice* pDevice{ nullptr };
			bool bBuffersCreated{ false };

			std::vector<FVertex> vVertices;
			std::unique_ptr<CBuffer> vertexBuffer;

			std::vector<uint32_t> vIndices;
			std::unique_ptr<CBuffer> indexBuffer;
		};
	}
}