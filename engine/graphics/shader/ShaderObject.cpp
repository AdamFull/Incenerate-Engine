#include "ShaderObject.h"

#include "APIHandle.h"
#include "image/Image.h"
#include "buffers/CommandBuffer.h"
#include "pipeline/GraphicsPipeline.h"
#include "pipeline/ComputePipeline.h"
#include "handlers/StorageHandler.h"
#include "handlers/UniformHandler.h"
#include "Engine.h"
#include "Helpers.h"

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

bool CShaderObject::isUsesBindlessTextures() const
{
	return usesBindlessTextures;
}

void CShaderObject::create(const FShaderCreateInfo& specials)
{
	auto* graphics = pDevice->getAPI();

	usesBindlessTextures = specials.use_bindles_textures;

	switch (specials.bind_point)
	{
	case vk::PipelineBindPoint::eGraphics: {
		pPipeline = std::make_unique<CGraphicsPipeline>(pDevice);
		pPipeline->create(this, graphics->getFramebuffer(specials.pipeline_stage)->getRenderPass(), specials);
	} break;
	case vk::PipelineBindPoint::eCompute: {
		pPipeline = std::make_unique<CComputePipeline>(pDevice);
		pPipeline->create(this, specials);
	} break;
	}

	for (auto& [name, push] : pShader->getPushBlocks())
	{
		auto pBlock = std::make_unique<CPushHandler>(pPipeline.get());
		pBlock->create(push);
		mPushBlocks.emplace(name, std::move(pBlock));
	}

	increaseUsage(specials.usages);
}

void CShaderObject::increaseUsage(size_t usages)
{
	for (size_t usage = 0; usage < usages; usage++)
	{
		auto pInstance = std::make_unique<FShaderInstance>();
		pInstance->pDescriptorSet = std::make_unique<CDescriptorHandler>(pDevice, pShader.get());
		pInstance->pDescriptorSet->create(pPipeline->getDescriptorPool(), pPipeline->getDescriptorSetLayouts());

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

	usageCount += usages;
}

void CShaderObject::bind(vk::CommandBuffer& commandBuffer)
{
	pPipeline->bind(commandBuffer);
}

void CShaderObject::bindDescriptor(vk::CommandBuffer& commandBuffer)
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

	for (auto& [name, push] : mPushBlocks)
		push->flush(commandBuffer);

	for (auto& [key, texture] : mTextures)
		pInstance->pDescriptorSet->set(key, texture);
	pInstance->pDescriptorSet->update();
	mTextures.clear();

	pInstance->pDescriptorSet->bind(commandBuffer, pPipeline->getBindPoint(), pPipeline->getPipelineLayout());

	currentUsage = (currentUsage + 1) % usageCount;
}

void CShaderObject::predraw(vk::CommandBuffer& commandBuffer)
{
	bindDescriptor(commandBuffer);
}

void CShaderObject::dispatch(const std::vector<FDispatchParam>& params)
{
	auto cmdBuf = CCommandBuffer(pDevice);
	cmdBuf.create(true, vk::QueueFlagBits::eCompute);
	auto& commandBuffer = cmdBuf.getCommandBuffer();

	dispatch(commandBuffer, params);
	
	cmdBuf.submitIdle();
}

void CShaderObject::dispatch(vk::CommandBuffer& commandBuffer, const std::vector<FDispatchParam>& params)
{
	auto& pInstance = vInstances.at(currentUsage);
	// TODO: add graphics pointer inside shader object
	auto graphics = pDevice->getAPI();

	bindDescriptor(commandBuffer);

	auto pipelineCount = pPipeline->count();

	// Dispatch all compute shaders with same descriptor
	for (size_t idx = 0; idx < pipelineCount; ++idx)
	{
		// Synchronize dispatches
		if (idx > 0)
			graphics->BarrierFromComputeToCompute(commandBuffer);

		auto& localSizes = pShader->getLocalSizes(idx);
		auto& param = params[idx];

		pPipeline->bind(commandBuffer, idx);

		
		uint32_t groupCountX{ 0 }, groupCountY{ 0 }, groupCountZ{ 0 };
		if (param.divideByLocalSizes)
		{
			groupCountX = static_cast<uint32_t>(std::ceil(static_cast<float>(param.size.x) / static_cast<float>(*localSizes[0])));
			groupCountY = static_cast<uint32_t>(std::ceil(static_cast<float>(param.size.y) / static_cast<float>(*localSizes[1])));
			groupCountZ = static_cast<uint32_t>(std::ceil(static_cast<float>(param.size.z) / static_cast<float>(*localSizes[2])));
		}
		else
		{
			groupCountX = static_cast<uint32_t>(param.size.x);
			groupCountY = static_cast<uint32_t>(param.size.y);
			groupCountZ = static_cast<uint32_t>(param.size.z);
		}

		groupCountX = glm::max(groupCountX, 1u);
		groupCountY = glm::max(groupCountY, 1u);
		groupCountZ = glm::max(groupCountZ, 1u);

		commandBuffer.dispatch(groupCountX, groupCountY, groupCountZ);
	}
}

void CShaderObject::dispatch(const FDispatchParam& param)
{
	std::vector<FDispatchParam> params{ param };
	dispatch(params);
}

void CShaderObject::dispatch(vk::CommandBuffer& commandBuffer, const FDispatchParam& param)
{
	std::vector<FDispatchParam> params{ param };
	dispatch(commandBuffer, params);
}

void CShaderObject::addTexture(const std::string& attachment, size_t id, uint32_t mip_level)
{
	auto graphics = pDevice->getAPI();
	auto& texture = graphics->getImage(id);

	vk::DescriptorImageInfo descriptor;

	if (texture && texture->isLoaded())
		descriptor = texture->getDescriptor(mip_level);
	else
	{
		auto& empty = graphics->getImage("empty_image_2d");
		descriptor = empty->getDescriptor();
	}

	addTexture(attachment, descriptor);
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
	static std::unique_ptr<CHandler> pEmpty{ nullptr };
	auto& pInstance = vInstances.at(currentUsage);

	auto uniformBlock = pInstance->mBuffers.find(name);
	if (uniformBlock != pInstance->mBuffers.end())
		return uniformBlock->second;
	return pEmpty;
}

const std::unique_ptr<CPushHandler>& CShaderObject::getPushBlock(const std::string& name)
{
	static std::unique_ptr<CPushHandler> pEmpty{ nullptr };
	auto pushBlock = mPushBlocks.find(name);
	if (pushBlock != mPushBlocks.end())
		return pushBlock->second;
	return pEmpty;
}