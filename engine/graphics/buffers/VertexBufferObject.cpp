#include "VertexBufferObject.h"

#include "APIHandle.h"

using namespace engine::graphics;

CVertexBufferObject::CVertexBufferObject(CDevice* device)
{
	pDevice = device;
}

void CVertexBufferObject::reserve(size_t vertex_size, size_t vertices, size_t index_size, size_t indices)
{
	if (vertices != 0)
		vertexBuffer = CBuffer::MakeVertexBuffer(pDevice, vertex_size, vertices);

	if (indices != 0)
		indexBuffer = CBuffer::MakeIndexBuffer(pDevice, index_size, indices);
}

void CVertexBufferObject::clear()
{
	last_vertex = 0;
	last_index = 0;
}

void CVertexBufferObject::bind(vk::CommandBuffer commandBuffer)
{
	// Has vertices
	if (last_vertex != 0ull)
	{
		{
			vk::DeviceSize offsets[] = { 0 };
			auto buffer = vertexBuffer->getBuffer();
			commandBuffer.bindVertexBuffers(0, 1, &buffer, offsets);
		}

		// Has indices
		if (last_index != 0ull)
		{
			auto buffer = indexBuffer->getBuffer();
			commandBuffer.bindIndexBuffer(buffer, 0, vk::IndexType::eUint32);
		}
	}
}

void CVertexBufferObject::addPrimitive(std::unique_ptr<FPrimitive>&& primitive)
{
	addMeshData(primitive->vVertices, primitive->vIndices);
}

void CVertexBufferObject::addMeshData(const std::vector<FVertex>& vertices, const std::vector<uint32_t>& indices)
{
	addVertices(vertices);
	addIndices(indices);
}

void CVertexBufferObject::copy_buffer_to_buffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, size_t size, size_t srcOffset, size_t dstOffset)
{
	pDevice->copyOnDeviceBuffer(srcBuffer, dstBuffer, size, srcOffset, dstOffset);
}

std::unique_ptr<CBuffer> CVertexBufferObject::allocate_buffer(size_t instance_size, size_t instances, uint32_t buffer_type)
{
	return buffer_type == 0 ? CBuffer::MakeVertexBuffer(pDevice, instance_size, instances) : CBuffer::MakeIndexBuffer(pDevice, instance_size, instances);
}

std::unique_ptr<CBuffer>& CVertexBufferObject::allocate_or_reallocate(size_t instance_size, size_t instances, size_t current_size, uint32_t buffer_type)
{
	auto& buffer = buffer_type == 0 ? vertexBuffer : indexBuffer;

	if (!buffer)
		buffer = allocate_buffer(instance_size, instances, buffer_type);
	else
	{
		// When we already have data
		if (current_size != 0)
		{
			auto tempBuffer = make_staging(instance_size, current_size);
			auto oldSize = instance_size * current_size;
			auto newCount = instances + current_size;

			// Copy old data to temporary buffer
			pDevice->copyOnDeviceBuffer(buffer->getBuffer(), tempBuffer->getBuffer(), oldSize);

			buffer = allocate_buffer(instance_size, newCount, buffer_type);

			// Copy old data back to vertex buffer
			pDevice->copyOnDeviceBuffer(tempBuffer->getBuffer(), buffer->getBuffer(), oldSize);
		}
	}

	return buffer;
}

std::unique_ptr<CBuffer> CVertexBufferObject::make_staging(size_t instance_size, size_t instances)
{
	return CBuffer::MakeStagingBuffer(pDevice, instance_size, instances);
}