#include "VertexBufferObject.h"

#include "APIHandle.h"

using namespace engine::graphics;

CVertexBufferObject::CVertexBufferObject(CDevice* device)
{
	pDevice = device;
}

void CVertexBufferObject::reserve(size_t position_stride, size_t color_stride, size_t normal_stride, size_t texcoord_stride, size_t tangent_stride, size_t joint_stride, size_t weight_stride, size_t vertices, size_t index_size, size_t indices)
{
	if (vertices != 0)
	{
		if (usage_mask & VERTEX_BUFFER_ATTRIBUTE_POSITION_BIT)
			positionBuffer = CBuffer::MakeVertexBuffer(pDevice, position_stride, vertices);
		if (usage_mask & VERTEX_BUFFER_ATTRIBUTE_COLOR_BIT)
			colorBuffer = CBuffer::MakeVertexBuffer(pDevice, color_stride, vertices);
		if (usage_mask & VERTEX_BUFFER_ATTRIBUTE_NORMAL_BIT)
			normalBuffer = CBuffer::MakeVertexBuffer(pDevice, normal_stride, vertices);
		if (usage_mask & VERTEX_BUFFER_ATTRIBUTE_TEXCOORD_BIT)
			texcoordBuffer = CBuffer::MakeVertexBuffer(pDevice, texcoord_stride, vertices);
		if (usage_mask & VERTEX_BUFFER_ATTRIBUTE_TANGENT_BIT)
			tangentBuffer = CBuffer::MakeVertexBuffer(pDevice, tangent_stride, vertices);
		if (usage_mask & VERTEX_BUFFER_ATTRIBUTE_JOINT_BIT)
			jointBuffer = CBuffer::MakeVertexBuffer(pDevice, joint_stride, vertices);
		if (usage_mask & VERTEX_BUFFER_ATTRIBUTE_WEIGHT_BIT)
			weightBuffer = CBuffer::MakeVertexBuffer(pDevice, weight_stride, vertices);
	}

	if (indices != 0)
		indexBuffer = CBuffer::MakeIndexBuffer(pDevice, index_size, indices);
}

void CVertexBufferObject::clear()
{
	last_position = 0;
	last_color = 0;
	last_normal = 0;
	last_texcoord = 0;
	last_tangent = 0;
	last_joint = 0;
	last_weight = 0;
	last_index = 0;
	bLoaded = false;
}

void CVertexBufferObject::bind(vk::CommandBuffer commandBuffer)
{
	// Has vertices
	if (last_position != 0ull)
	{
		{
			vk::DeviceSize offsets[] = { 0 };

			if (usage_mask & VERTEX_BUFFER_ATTRIBUTE_POSITION_BIT)
			{
				auto bufferPos = positionBuffer->getBuffer();
				commandBuffer.bindVertexBuffers(0, 1, &bufferPos, offsets);
			}

			if (usage_mask & VERTEX_BUFFER_ATTRIBUTE_COLOR_BIT)
			{
				auto bufferCol = colorBuffer->getBuffer();
				commandBuffer.bindVertexBuffers(1, 1, &bufferCol, offsets);
			}

			if (usage_mask & VERTEX_BUFFER_ATTRIBUTE_NORMAL_BIT)
			{
				auto bufferNorm = normalBuffer->getBuffer();
				commandBuffer.bindVertexBuffers(2, 1, &bufferNorm, offsets);
			}

			if (usage_mask & VERTEX_BUFFER_ATTRIBUTE_TEXCOORD_BIT)
			{
				auto bufferTex = texcoordBuffer->getBuffer();
				commandBuffer.bindVertexBuffers(3, 1, &bufferTex, offsets);
			}

			if (usage_mask & VERTEX_BUFFER_ATTRIBUTE_TANGENT_BIT)
			{
				auto bufferTan = tangentBuffer->getBuffer();
				commandBuffer.bindVertexBuffers(4, 1, &bufferTan, offsets);
			}

			if (usage_mask & VERTEX_BUFFER_ATTRIBUTE_JOINT_BIT)
			{
				auto bufferJo = jointBuffer->getBuffer();
				commandBuffer.bindVertexBuffers(5, 1, &bufferJo, offsets);
			}

			if (usage_mask & VERTEX_BUFFER_ATTRIBUTE_WEIGHT_BIT)
			{
				auto bufferWei = weightBuffer->getBuffer();
				commandBuffer.bindVertexBuffers(6, 1, &bufferWei, offsets);
			}
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
	addMeshData(primitive->position, primitive->color, primitive->normal, primitive->texcoord, primitive->tangent, primitive->joint, primitive->weight, primitive->vIndices);
}

void CVertexBufferObject::addMeshData(const std::vector<glm::vec3>& position, const std::vector<glm::vec3>& color, const std::vector<glm::vec3>& normal,
	const std::vector<glm::vec2>& texcoord, const std::vector<glm::vec4>& tangent, const std::vector<glm::vec4>& joint, const std::vector<glm::vec4>& weight,
	const std::vector<uint32_t>& indices)
{
	addVertices(position, 0);
	addVertices(color, 1);
	addVertices(normal, 2);
	addVertices(texcoord, 3);
	addVertices(tangent, 4);
	addVertices(joint, 5);
	addVertices(weight, 6);
	addIndices(indices);
}

CDevice* CVertexBufferObject::getDevice()
{
	return pDevice;
}

void CVertexBufferObject::copy_buffer_to_buffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, size_t size, size_t srcOffset, size_t dstOffset)
{
	pDevice->copyOnDeviceBuffer(srcBuffer, dstBuffer, size, srcOffset, dstOffset);
}

std::unique_ptr<CBuffer> CVertexBufferObject::allocate_buffer(size_t instance_size, size_t instances, uint32_t buffer_type)
{
	if(buffer_type == 7)
		return CBuffer::MakeIndexBuffer(pDevice, instance_size, instances);
	return CBuffer::MakeVertexBuffer(pDevice, instance_size, instances);
}

std::unique_ptr<CBuffer>& CVertexBufferObject::allocate_or_reallocate(size_t instance_size, size_t instances, size_t current_size, uint32_t buffer_type)
{
	std::unique_ptr<CBuffer>* buffer{ nullptr };
	switch (buffer_type)
	{
	case 0: buffer = &positionBuffer; break;
	case 1: buffer = &colorBuffer; break;
	case 2: buffer = &normalBuffer; break;
	case 3: buffer = &texcoordBuffer; break;
	case 4: buffer = &tangentBuffer; break;
	case 5: buffer = &jointBuffer; break;
	case 6: buffer = &weightBuffer; break;
	case 7: buffer = &indexBuffer; break;
	}

	if (!(*buffer))
		*buffer = allocate_buffer(instance_size, instances, buffer_type);
	else
	{
		// When we already have data
		if (current_size != 0)
		{
			auto tempBuffer = make_staging(instance_size, current_size);
			auto oldSize = instance_size * current_size;
			auto newCount = instances + current_size;

			// Copy old data to temporary buffer
			pDevice->copyOnDeviceBuffer((*buffer)->getBuffer(), tempBuffer->getBuffer(), oldSize);

			*buffer = allocate_buffer(instance_size, newCount, buffer_type);

			// Copy old data back to vertex buffer
			pDevice->copyOnDeviceBuffer(tempBuffer->getBuffer(), (*buffer)->getBuffer(), oldSize);
		}
	}

	return *buffer;
}

std::unique_ptr<CBuffer> CVertexBufferObject::make_staging(size_t instance_size, size_t instances)
{
	return CBuffer::MakeStagingBuffer(pDevice, instance_size, instances);
}