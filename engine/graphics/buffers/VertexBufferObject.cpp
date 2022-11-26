#include "VertexBufferObject.h"

#include "APIHandle.h"

using namespace engine::graphics;

CVertexBufferObject::CVertexBufferObject(CDevice* device)
{
	pDevice = device;
}

void CVertexBufferObject::create()
{
	if (!bBuffersCreated)
	{
		createVertexBuffer();
		createIndexBuffer();
		bBuffersCreated = true;
	}
}

void CVertexBufferObject::bind(vk::CommandBuffer commandBuffer)
{
	if (!bBuffersCreated)
		create();

	auto hasVertices = !vVertices.empty();

	if (hasVertices)
	{
		auto hasIndex = !vIndices.empty();

		vk::DeviceSize offsets[] = { 0 };
		auto buffer = vertexBuffer->getBuffer();
		commandBuffer.bindVertexBuffers(0, 1, &buffer, offsets);

		if (hasIndex)
		{
			commandBuffer.bindIndexBuffer(indexBuffer->getBuffer(), 0, vk::IndexType::eUint32);
			commandBuffer.drawIndexed(vIndices.size(), 1, 0, 0, 0);
		}
		else
			commandBuffer.draw(static_cast<uint32_t>(vVertices.size()), 1, 0, 0);
	}
	else
		commandBuffer.draw(3, 1, 0, 0);
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
inline uint64_t CVertexBufferObject::getLastIndex()
{
	return vIndices.size();
}

inline uint64_t CVertexBufferObject::getLastVertex()
{
	return vVertices.size();
}

void CVertexBufferObject::createVertexBuffer()
{
	if (!vVertices.empty())
	{
		vk::DeviceSize bufferSize = sizeof(vVertices[0]) * vVertices.size();
		auto vertexSize = sizeof(vVertices[0]);

		auto stagingBuffer = CBuffer::MakeStagingBuffer(pDevice, vertexSize, vVertices.size());
		stagingBuffer->map();
		stagingBuffer->write((void*)vVertices.data());

		vertexBuffer = CBuffer::MakeVertexBuffer(pDevice, vertexSize, vVertices.size());
		pDevice->copyOnDeviceBuffer(stagingBuffer->getBuffer(), vertexBuffer->getBuffer(), bufferSize);
	}
}

void CVertexBufferObject::createIndexBuffer()
{
	if (!vIndices.empty())
	{
		vk::DeviceSize bufferSize = sizeof(vIndices[0]) * vIndices.size();
		auto indexSize = sizeof(vIndices[0]);

		auto stagingBuffer = CBuffer::MakeStagingBuffer(pDevice, indexSize, vIndices.size());
		stagingBuffer->map();
		stagingBuffer->write((void*)vIndices.data());

		indexBuffer = CBuffer::MakeIndexBuffer(pDevice, indexSize, vIndices.size());
		pDevice->copyOnDeviceBuffer(stagingBuffer->getBuffer(), indexBuffer->getBuffer(), bufferSize);
	}
}