#include "VertexBufferObject.h"

#include "APIHandle.h"

using namespace engine::graphics;

CVertexBufferObject::CVertexBufferObject(CDevice* device)
{
	pDevice = device;
}

void CVertexBufferObject::create()
{
	recreate();
}

void CVertexBufferObject::create(size_t vertices, size_t indices)
{
	if (vertices != 0)
	{
		auto vertexSize = sizeof(FVertex);
		vertexBuffer = CBuffer::MakeVertexBuffer(pDevice, vertexSize, vertices);
	}
	
	if (indices != 0)
	{
		auto indexSize = sizeof(uint32_t);
		indexBuffer = CBuffer::MakeIndexBuffer(pDevice, indexSize, vIndices.size());
	}
}

void CVertexBufferObject::update(std::vector<FVertex>& vertices)
{
	vVertices = vertices;
	createVertexBuffer();
}

void CVertexBufferObject::update(std::vector<uint32_t>& indices)
{
	vIndices = indices;
	createIndexBuffer();
}

void CVertexBufferObject::update(std::vector<FVertex>& vertices, std::vector<uint32_t>& indices)
{
	update(vertices);
	update(indices);
}

void CVertexBufferObject::recreate()
{
	createVertexBuffer();
	createIndexBuffer();
}

void CVertexBufferObject::bind(vk::CommandBuffer commandBuffer)
{
	auto hasVertices = !vVertices.empty();

	if (hasVertices)
	{
		auto hasIndex = !vIndices.empty();

		vk::DeviceSize offsets[] = { 0 };
		auto buffer = vertexBuffer->getBuffer();
		commandBuffer.bindVertexBuffers(0, 1, &buffer, offsets);

		if (hasIndex)
			commandBuffer.bindIndexBuffer(indexBuffer->getBuffer(), 0, vk::IndexType::eUint32);
	}
}

void CVertexBufferObject::addPrimitive(std::unique_ptr<FPrimitive>&& primitive)
{
	addMeshData(std::move(primitive->vVertices), std::move(primitive->vIndices));
}

void CVertexBufferObject::addVertices(std::vector<FVertex>&& vertices)
{
	vVertices.insert(vVertices.end(), vertices.begin(), vertices.end());
}

void CVertexBufferObject::addIndices(std::vector<uint32_t>&& indices)
{
	vIndices.insert(vIndices.end(), indices.begin(), indices.end());
}

void CVertexBufferObject::addMeshData(std::vector<FVertex>&& vertices, std::vector<uint32_t>&& indices)
{
	addVertices(std::move(vertices));
	addIndices(std::move(indices));
}
uint64_t CVertexBufferObject::getLastIndex()
{
	return vIndices.size();
}

uint64_t CVertexBufferObject::getLastVertex()
{
	return vVertices.size();
}

void CVertexBufferObject::createVertexBuffer()
{
	if (!vVertices.empty())
	{
		auto vertexSize = sizeof(FVertex);
		vk::DeviceSize bufferSize = vertexSize * vVertices.size();

		auto stagingBuffer = CBuffer::MakeStagingBuffer(pDevice, vertexSize, vVertices.size());
		stagingBuffer->map();
		stagingBuffer->write((void*)vVertices.data());

		if(!vertexBuffer)
			vertexBuffer = CBuffer::MakeVertexBuffer(pDevice, vertexSize, vVertices.size());

		pDevice->copyOnDeviceBuffer(stagingBuffer->getBuffer(), vertexBuffer->getBuffer(), bufferSize);
	}
}

void CVertexBufferObject::createIndexBuffer()
{
	if (!vIndices.empty())
	{
		auto indexSize = sizeof(uint32_t);
		vk::DeviceSize bufferSize = indexSize * vIndices.size();

		auto stagingBuffer = CBuffer::MakeStagingBuffer(pDevice, indexSize, vIndices.size());
		stagingBuffer->map();
		stagingBuffer->write((void*)vIndices.data());

		if(!indexBuffer)
			indexBuffer = CBuffer::MakeIndexBuffer(pDevice, indexSize, vIndices.size());

		pDevice->copyOnDeviceBuffer(stagingBuffer->getBuffer(), indexBuffer->getBuffer(), bufferSize);
	}
}