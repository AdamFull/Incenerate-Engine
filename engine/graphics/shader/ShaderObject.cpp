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

void CShaderObject::create(uint32_t subpass, size_t usages)
{
	usageCount = usages;

	//Creating pipeline
	switch (programCI.bindPoint)
	{
	case vk::PipelineBindPoint::eGraphics: {
		pPipeline = std::make_unique<CGraphicsPipeline>(pDevice);
		pPipeline->create(this, EGGraphics->getFramebuffer(programCI.srStage)->getRenderPass(), subpass);
	} break;
	case vk::PipelineBindPoint::eCompute: {
		pPipeline = std::make_unique<CComputePipeline>(pDevice);
		pPipeline->create(this);
	} break;
	}

	for (size_t usage = 0; usage < usageCount; usage++)
	{
		auto pInstance = std::make_unique<FShaderInstance>();
		pInstance->pDescriptorSet = std::make_unique<CDescriptorHandler>(pDevice);
		pInstance->pDescriptorSet->create(this);

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

			pInstance->mBuffers.emplace(name, std::move(pUniform));
		}
		vInstances.emplace_back(std::move(pInstance));
	}
	

	for (auto& [name, push] : pShader->getPushBlocks())
	{
		auto pBlock = std::make_unique<CPushHandler>(pPipeline.get());
		pBlock->create(push);
		mPushBlocks.emplace(name, std::move(pBlock));
	}
}

void CShaderObject::predraw(vk::CommandBuffer& commandBuffer)
{
	auto& pInstance = vInstances.at(currentUsage);

	pInstance->pDescriptorSet->reset();
	for (auto& [name, uniform] : pInstance->mBuffers)
	{
		auto& buffer = uniform->getBuffer();
		auto& descriptor = buffer->getDescriptor();
		pInstance->pDescriptorSet->set(name, descriptor);
		uniform->flush();
	}

	for (auto& [key, texture] : mTextures)
		pInstance->pDescriptorSet->set(key, texture);
	pInstance->pDescriptorSet->update();
	mTextures.clear();

	pInstance->pDescriptorSet->bind(commandBuffer);
	pPipeline->bind(commandBuffer);

	currentUsage = (currentUsage + 1) % usageCount;
}

void CShaderObject::dispatch(size_t size)
{
	auto cmdBuf = CCommandBuffer(pDevice);
	cmdBuf.create(true, vk::QueueFlagBits::eCompute);
	auto& commandBuffer = cmdBuf.getCommandBuffer();

	dispatch(commandBuffer, size);
	
	cmdBuf.submitIdle();
}

void CShaderObject::dispatch(vk::CommandBuffer& commandBuffer, size_t size)
{
	predraw(commandBuffer);

	auto groupCountX = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*pShader->getLocalSizes()[0])));
	auto groupCountY = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*pShader->getLocalSizes()[1])));
	commandBuffer.dispatch(groupCountX, groupCountY, 1);
}

void CShaderObject::addTexture(const std::string& attachment, size_t id)
{
	auto& texture = EGGraphics->getImage(id);
	addTexture(attachment, texture->getDescriptor());
}

void CShaderObject::addTexture(const std::string& attachment, vk::DescriptorImageInfo descriptor)
{
	mTextures[attachment] = descriptor;
}

vk::DescriptorImageInfo& CShaderObject::getTexture(const std::string& attachment)
{
	return mTextures[attachment];
}

const std::unique_ptr<CHandler>& CShaderObject::getUniformBuffer(const std::string& name)
{
	auto& pInstance = vInstances.at(currentUsage);

	auto uniformBlock = pInstance->mBuffers.find(name);
	if (uniformBlock != pInstance->mBuffers.end())
		return uniformBlock->second;
	return nullptr;
}

const std::unique_ptr<CPushHandler>& CShaderObject::getPushBlock(const std::string& name)
{
	auto pushBlock = mPushBlocks.find(name);
	if (pushBlock != mPushBlocks.end())
		return pushBlock->second;
	return nullptr;
}