#pragma once

#include "Buffer.h"
#include "APIStructures.h"

namespace engine
{
	namespace graphics
	{
		struct FPrimitive
		{
			std::vector<glm::vec3> position;
			std::vector<glm::vec3> color;
			std::vector<glm::vec3> normal;
			std::vector<glm::vec2> texcoord;
			std::vector<glm::vec4> tangent;
			std::vector<glm::vec4> joint;
			std::vector<glm::vec4> weight;
			std::vector<uint32_t> vIndices;
		};

		struct FCubePrimitive : public FPrimitive
		{
			FCubePrimitive()
			{
				position = {
					glm::vec3(-5, -5, 5),
					glm::vec3(-5, -5, -5),
					glm::vec3(5, -5, -5),
					glm::vec3(5, -5, 5),
					glm::vec3(-5, 5, 5),
					glm::vec3(5, 5, 5),
					glm::vec3(5, 5, -5),
					glm::vec3(-5, 5, -5),
					glm::vec3(-5, -5, 5),
					glm::vec3(5, -5, 5),
					glm::vec3(5, 5, 5),
					glm::vec3(-5, 5, 5),
					glm::vec3(5, -5, 5),
					glm::vec3(5, -5, -5),
					glm::vec3(5, 5, -5),
					glm::vec3(5, 5, 5),
					glm::vec3(5, -5, -5),
					glm::vec3(-5, -5, -5),
					glm::vec3(-5, 5, -5),
					glm::vec3(5, 5, -5),
					glm::vec3(-5, -5, -5),
					glm::vec3(-5, -5, 5),
					glm::vec3(-5, 5, 5),
					glm::vec3(-5, 5, -5)
				};

				normal = {
					glm::vec3(0, -1, 0),
					glm::vec3(0, -1, 0),
					glm::vec3(0, -1, 0),
					glm::vec3(0, -1, 0),
					glm::vec3(0, 1, 0),
					glm::vec3(0, 1, 0),
					glm::vec3(0, 1, 0),
					glm::vec3(0, 1, 0),
					glm::vec3(0, -1, 0),
					glm::vec3(0, -1, 0),
					glm::vec3(0, 1, 0),
					glm::vec3(0, 1, 0),
					glm::vec3(0, -1, 0),
					glm::vec3(0, -1, 0),
					glm::vec3(0, 1, 0),
					glm::vec3(0, 1, 0),
					glm::vec3(0, -1, 0),
					glm::vec3(0, -1, 0),
					glm::vec3(0, 1, 0),
					glm::vec3(0, 1, 0),
					glm::vec3(0, -1, 0),
					glm::vec3(0, -1, 0),
					glm::vec3(0, 1, 0),
					glm::vec3(0, 1, 0),
				};

				texcoord = {
					glm::vec2(1, 1),
					glm::vec2(1, 0),
					glm::vec2(0, 0),
					glm::vec2(0, 1),
					glm::vec2(0, 1),
					glm::vec2(1, 1),
					glm::vec2(1, 0),
					glm::vec2(0, 0),
					glm::vec2(0, 1),
					glm::vec2(1, 1),
					glm::vec2(1, 0),
					glm::vec2(0, 0),
					glm::vec2(0, 1),
					glm::vec2(1, 1),
					glm::vec2(1, 0),
					glm::vec2(0, 0),
					glm::vec2(0, 1),
					glm::vec2(1, 1),
					glm::vec2(1, 0),
					glm::vec2(0, 0),
					glm::vec2(0, 1),
					glm::vec2(1, 1),
					glm::vec2(1, 0),
					glm::vec2(0, 0)
				};

				color.resize(position.size(), glm::vec3{1.f});
				tangent.resize(position.size());
				joint.resize(position.size());
				weight.resize(position.size());

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
				position = {
					glm::vec3(-1.f, 1.f, 0),
					glm::vec3(1.f, 1.f, 0),
					glm::vec3(-1.f, -1.f, 0),
					glm::vec3(1.f, -1.f, 0)
				};

				normal = {
					glm::vec3(0, 1.f, 0),
					glm::vec3(0, 1.f, 0),
					glm::vec3(0, 1.f, 0),
					glm::vec3(0, 1.f, 0),
				};

				texcoord = {
					glm::vec2(0.f, 0.f),
					glm::vec2(1.f, 0.f),
					glm::vec2(0.f, 1.f),
					glm::vec2(1.f, 1.f),
				};

				color.resize(position.size(), glm::vec3{ 1.f });
				tangent.resize(position.size());
				joint.resize(position.size());
				weight.resize(position.size());

				vIndices = {0, 1, 3, 0, 3, 2};
			}
		};

		enum EVertexBufferAttributeBits : uint32_t
		{
			VERTEX_BUFFER_ATTRIBUTE_POSITION_BIT = 1 << 0,
			VERTEX_BUFFER_ATTRIBUTE_COLOR_BIT = 1 << 1,
			VERTEX_BUFFER_ATTRIBUTE_NORMAL_BIT = 1 << 2,
			VERTEX_BUFFER_ATTRIBUTE_TEXCOORD_BIT = 1 << 3,
			VERTEX_BUFFER_ATTRIBUTE_TANGENT_BIT = 1 << 4,
			VERTEX_BUFFER_ATTRIBUTE_JOINT_BIT = 1 << 5,
			VERTEX_BUFFER_ATTRIBUTE_WEIGHT_BIT = 1 << 6,
			VERTEX_BUFFER_ATTRIBUTE_ALL_BIT = VERTEX_BUFFER_ATTRIBUTE_POSITION_BIT | VERTEX_BUFFER_ATTRIBUTE_COLOR_BIT | 
											  VERTEX_BUFFER_ATTRIBUTE_NORMAL_BIT | VERTEX_BUFFER_ATTRIBUTE_TEXCOORD_BIT | 
											  VERTEX_BUFFER_ATTRIBUTE_TANGENT_BIT | VERTEX_BUFFER_ATTRIBUTE_JOINT_BIT |
											  VERTEX_BUFFER_ATTRIBUTE_WEIGHT_BIT
		};

		class CVertexBufferObject
		{
		public:
			CVertexBufferObject() = default;
			CVertexBufferObject(CDevice* device);
			~CVertexBufferObject() = default;

			void reserve(size_t position_stride, size_t color_stride, size_t normal_stride, size_t texcoord_stride, size_t tangent_stride, size_t joint_stride, size_t weight_stride, size_t vertices, size_t index_size, size_t indices);
			void clear();

			void set_usage_mask(uint32_t usage) { usage_mask = usage; }

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
			void addVertices(const std::vector<_Ty>& vertices, uint32_t buffer_type)
			{
				size_t* last_vertex{ nullptr };
				switch (buffer_type)
				{
				case 0: last_vertex = &last_position; break;
				case 1: last_vertex = &last_color; break;
				case 2: last_vertex = &last_normal; break;
				case 3: last_vertex = &last_texcoord; break;
				case 4: last_vertex = &last_tangent; break;
				case 5: last_vertex = &last_joint; break;
				case 6: last_vertex = &last_weight; break;
				}
				addData(vertices, *last_vertex, buffer_type);
			}

			void addIndices(const std::vector<uint32_t>& indices)
			{
				addData(indices, last_index, 7);
			}

			void bind(vk::CommandBuffer commandBuffer);
			void addPrimitive(std::unique_ptr<FPrimitive>&& primitive);
			void addMeshData(
				const std::vector<glm::vec3>& position, 
				const std::vector<glm::vec3>& color, 
				const std::vector<glm::vec3>& normal,
				const std::vector<glm::vec2>& texcoord,
				const std::vector<glm::vec4>& tangent,
				const std::vector<glm::vec4>& joint,
				const std::vector<glm::vec4>& weight,
				const std::vector<uint32_t>& indices);
			const size_t getLastIndex() const { return last_index; }
			const size_t getLastVertex() const { return last_position; }

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

			uint32_t usage_mask{ VERTEX_BUFFER_ATTRIBUTE_ALL_BIT };

			std::unique_ptr<CBuffer> positionBuffer;
			size_t last_position{ 0 };

			std::unique_ptr<CBuffer> colorBuffer;
			size_t last_color{ 0 };

			std::unique_ptr<CBuffer> normalBuffer;
			size_t last_normal{ 0 };

			std::unique_ptr<CBuffer> texcoordBuffer;
			size_t last_texcoord{ 0 };

			std::unique_ptr<CBuffer> tangentBuffer;
			size_t last_tangent{ 0 };

			std::unique_ptr<CBuffer> jointBuffer;
			size_t last_joint{ 0 };

			std::unique_ptr<CBuffer> weightBuffer;
			size_t last_weight{ 0 };

			std::unique_ptr<CBuffer> indexBuffer;
			size_t last_index{ 0 };
		};
	}
}