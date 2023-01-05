#include "ShaderObject.h"

#include "Device.h"
#include "image/Image.h"
#include "buffers/CommandBuffer.h"
#include "pipeline/GraphicsPipeline.h"
#include "pipeline/ComputePipeline.h"
#include "handlers/StorageHandler.h"
#include "handlers/UniformHandler.h"
#include "Engine.h"

using namespace engine;
using namespace engine::graphics;

CShaderObject::CShaderObject(CDevice* device)
{
	pDevice = device;

	pShader = std::make_unique<CShader>(pDevice);
}

CShaderObject::~CShaderObject()
{

}

const std::unique_ptr<CShader>& CShaderObject::getShader()
{
	return pShader;
}

void CShaderObject::create()
{
	auto& framebuffer = pDevice->getAPI()->getFramebuffer(programCI.srStage);
	if (framebuffer)
	{
		//Creating pipeline
		switch (programCI.bindPoint)
		{
		case vk::PipelineBindPoint::eGraphics: {
			pPipeline = std::make_unique<CGraphicsPipeline>(pDevice);
			pPipeline->create(this, framebuffer->getRenderPass(), 0);
		} break;
		case vk::PipelineBindPoint::eCompute: {
			pPipeline = std::make_unique<CComputePipeline>(pDevice);
			pPipeline->create(this);
		} break;
		}

		pDescriptorSet = std::make_unique<CDescriptorHandler>(pDevice);
		pDescriptorSet->create(this);

		for (auto& [name, uniform] : pShader->getUniformBlocks())
		{
			std::unique_ptr<CHandler> pUniform;
			switch (uniform.getDescriptorType())
			{
			case vk::DescriptorType::eUniformBuffer: {
				pUniform = std::make_unique<CUniformHandler>(pDevice);
				pUniform->create(uniform);
			} break;
			case vk::DescriptorType::eStorageBuffer: {
				pUniform = std::make_unique<CStorageHandler>(pDevice);
				pUniform->create(uniform);
			} break;
			}
		}
	}
}

void CShaderObject::predraw(vk::CommandBuffer& commandBuffer)
{
	pDescriptorSet->reset();
	for (auto& [name, uniform] : mBuffers)
	{
		auto& buffer = uniform->getBuffer();
		auto& descriptor = buffer->getDescriptor();
		pDescriptorSet->set(name, descriptor);
		uniform->flush();
	}

	for (auto& [key, texture] : mTextures)
		pDescriptorSet->set(key, texture);
	pDescriptorSet->update();

	pDescriptorSet->bind(commandBuffer);
	pPipeline->bind(commandBuffer);
}

void CShaderObject::dispatch(size_t size)
{
	auto cmdBuf = CCommandBuffer(pDevice);
	cmdBuf.create(true, vk::QueueFlagBits::eCompute);
	auto& commandBuffer = cmdBuf.getCommandBuffer();

	auto groupCountX = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*pShader->getLocalSizes()[0])));
	auto groupCountY = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*pShader->getLocalSizes()[1])));
	commandBuffer.dispatch(groupCountX, groupCountY, 1);
	cmdBuf.submitIdle();
}

void CShaderObject::addTexture(const std::string& attachment, size_t id)
{
	auto& texture = EGGraphics->getImage(id);
	mTextures[attachment] = texture->getDescriptor();
}

vk::DescriptorImageInfo& CShaderObject::getTexture(const std::string& attachment)
{
	return mTextures[attachment];
}

std::unique_ptr<CHandler>& CShaderObject::getUniformBuffer(const std::string& name)
{
	auto uniformBlock = mBuffers.find(name);
	if (uniformBlock != mBuffers.end())
		return uniformBlock->second;
	return pEmptyHandler;
}