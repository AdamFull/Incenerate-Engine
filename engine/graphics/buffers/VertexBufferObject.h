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

			void reserve(size_t vertex_size, size_t vertices, size_t index_size, size_t indices);
			void clear();

			template<class _Ty>
			void addData(const std::vector<_Ty>& data, size_t& last_point, uint32_t type)
			{
				if (!data.empty())
				{
					auto instance_size = sizeof(_Ty);
					auto instances = data.size();

					auto& target = allocate_or_reallocate(instance_size, instances, last_point, type);

					auto stagingBuffer = make_staging(instance_size, instances);
					stagingBuffer->map();
					stagingBuffer->write((void*)data.data());

					copy_buffer_to_buffer(stagingBuffer->getBuffer(), target->getBuffer(), instance_size * instances, 0, last_point * instance_size);

					last_point += instances;
				}
			}

			template<class _Ty>
			void addVertices(const std::vector<_Ty>& vertices)
			{
				addData(vertices, last_vertex, 0);
			}

			void addIndices(const std::vector<uint32_t>& indices)
			{
				addData(indices, last_index, 1);
			}

			void bind(vk::CommandBuffer commandBuffer);
			void addPrimitive(std::unique_ptr<FPrimitive>&& primitive);
			void addMeshData(const std::vector<FVertex>& vertices, const std::vector<uint32_t>& indices);
			const size_t getLastIndex() const { return last_index; }
			const size_t getLastVertex() const { return last_vertex; }

			void setLoaded() { bLoaded = true; }
			const bool isLoaded() const { return bLoaded; }

			CDevice* getDevice();

		private:
			void copy_buffer_to_buffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, size_t size, size_t srcOffset, size_t dstOffset);
			std::unique_ptr<CBuffer> allocate_buffer(size_t instance_size, size_t instances, uint32_t buffer_type);
			std::unique_ptr<CBuffer>& allocate_or_reallocate(size_t instance_size, size_t instances, size_t current_size, uint32_t buffer_type);
			std::unique_ptr<CBuffer> make_staging(size_t instance_size, size_t instances);

		private:
			CDevice* pDevice{ nullptr };
			bool bBuffersCreated{ false };
			bool bLoaded{ false };

			std::unique_ptr<CBuffer> vertexBuffer;
			size_t last_vertex{ 0 };

			std::unique_ptr<CBuffer> indexBuffer;
			size_t last_index{ 0 };
		};
	}
}