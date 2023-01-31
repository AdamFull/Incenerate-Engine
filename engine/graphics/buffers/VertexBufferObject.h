#pragma once

#include "Buffer.h"
#include "APIStructures.h"

namespace engine
{
	namespace graphics
	{
		struct FPrimitive
		{
			std::vector<FVertex> vVertices;
			std::vector<uint32_t> vIndices;
		};

		struct FCubePrimitive : public FPrimitive
		{
			FCubePrimitive()
			{
				vVertices =
				{
					FVertex(glm::vec3(-5, -5, 5), glm::vec3(0, -1, 0), glm::vec2(1, 1)),
					FVertex(glm::vec3(-5, -5, -5), glm::vec3(0, -1, 0), glm::vec2(1, 0)),
					FVertex(glm::vec3(5, -5, -5), glm::vec3(0, -1, 0), glm::vec2(0, 0)),
					FVertex(glm::vec3(5, -5, 5), glm::vec3(0, -1, 0), glm::vec2(0, 1)),
					FVertex(glm::vec3(-5, 5, 5), glm::vec3(0, 1, 0), glm::vec2(0, 1)),
					FVertex(glm::vec3(5, 5, 5), glm::vec3(0, 1, 0), glm::vec2(1, 1)),
					FVertex(glm::vec3(5, 5, -5), glm::vec3(0, 1, 0), glm::vec2(1, 0)),
					FVertex(glm::vec3(-5, 5, -5), glm::vec3(0, 1, 0), glm::vec2(0, 0)),
					FVertex(glm::vec3(-5, -5, 5), glm::vec3(0, -1, 0), glm::vec2(0, 1)),
					FVertex(glm::vec3(5, -5, 5), glm::vec3(0, -1, 0), glm::vec2(1, 1)),
					FVertex(glm::vec3(5, 5, 5), glm::vec3(0, 1, 0), glm::vec2(1, 0)),
					FVertex(glm::vec3(-5, 5, 5), glm::vec3(0, 1, 0), glm::vec2(0, 0)),
					FVertex(glm::vec3(5, -5, 5), glm::vec3(0, -1, 0), glm::vec2(0, 1)),
					FVertex(glm::vec3(5, -5, -5), glm::vec3(0, -1, 0), glm::vec2(1, 1)),
					FVertex(glm::vec3(5, 5, -5), glm::vec3(0, 1, 0), glm::vec2(1, 0)),
					FVertex(glm::vec3(5, 5, 5), glm::vec3(0, 1, 0), glm::vec2(0, 0)),
					FVertex(glm::vec3(5, -5, -5), glm::vec3(0, -1, 0), glm::vec2(0, 1)),
					FVertex(glm::vec3(-5, -5, -5), glm::vec3(0, -1, 0), glm::vec2(1, 1)),
					FVertex(glm::vec3(-5, 5, -5), glm::vec3(0, 1, 0), glm::vec2(1, 0)),
					FVertex(glm::vec3(5, 5, -5), glm::vec3(0, 1, 0), glm::vec2(0, 0)),
					FVertex(glm::vec3(-5, -5, -5), glm::vec3(0, -1, 0), glm::vec2(0, 1)),
					FVertex(glm::vec3(-5, -5, 5), glm::vec3(0, -1, 0), glm::vec2(1, 1)),
					FVertex(glm::vec3(-5, 5, 5), glm::vec3(0, 1, 0), glm::vec2(1, 0)),
					FVertex(glm::vec3(-5, 5, -5), glm::vec3(0, 1, 0), glm::vec2(0, 0)),

				};

				vIndices = 
				{ 
					0, 1, 2, 2, 3, 0,
					4, 5, 6, 6, 7, 4, 
					8, 9, 10, 10, 11, 8, 
					12, 13, 14 ,14, 15, 12, 
					16, 17, 18, 18, 19, 16, 
					20, 21, 22, 22, 23, 20 
				};
			}
		};

		struct FQuadPrimitive : public FPrimitive
		{
			FQuadPrimitive()
			{
				vVertices =
				{
					FVertex(glm::vec3(-1.f, 1.f, 0), glm::vec3(0, 1.f, 0), glm::vec2(0.f, 0.f)),
					FVertex(glm::vec3(1.f, 1.f, 0), glm::vec3(0, 1.f, 0), glm::vec2(1.f, 0.f)),
					FVertex(glm::vec3(-1.f, -1.f, 0), glm::vec3(0, 1.f, 0), glm::vec2(0.f, 1.f)),
					FVertex(glm::vec3(1.f, -1.f, 0), glm::vec3(0, 1.f, 0), glm::vec2(1.f, 1.f)),
				};

				vIndices = {0, 1, 3, 0, 3, 2};
			}
		};

		class CVertexBufferObject
		{
		public:
			CVertexBufferObject() = default;
			CVertexBufferObject(CDevice* device);
			~CVertexBufferObject() = default;

			void create();
			void create(size_t vertices, size_t indices);
			void update(std::vector<FVertex>& vertices);
			void update(std::vector<uint32_t>& indices);
			void update(std::vector<FVertex>& vertices, std::vector<uint32_t>& indices);
			void recreate();
			void bind(vk::CommandBuffer commandBuffer);
			void addPrimitive(std::unique_ptr<FPrimitive>&& primitive);
			void addVertices(std::vector<FVertex>&& vertices);
			void addIndices(std::vector<uint32_t>&& indices);
			void addMeshData(std::vector<FVertex>&& vertices, std::vector<uint32_t>&& indices);
			uint64_t getLastIndex();
			uint64_t getLastVertex();

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