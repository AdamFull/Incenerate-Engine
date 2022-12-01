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
    if (!bIsCreated)
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

            for (auto instance = 0; instance < instances; instance++)
            {
                auto instance_ptr = std::make_unique<FSOInstance>();
                instance_ptr->pDescriptorSet = std::make_unique<CDescriptorHandler>(pDevice);
                instance_ptr->pDescriptorSet->create(this);

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
                    instance_ptr->mBuffers.emplace(name, std::move(pUniform));
                }

                vInstances.emplace_back(std::move(instance_ptr));
            }
        }

        bIsCreated = true;
    }
}

void CShaderObject::reCreate()
{
    
}

void CShaderObject::render(vk::CommandBuffer& commandBuffer)
{
    auto& pDescriptorSet = getDescriptorSet();
    auto& mBuffers = getUniformBuffers();

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
    bIsReCreated = false;
    currentInstance = (currentInstance + 1) % instances;
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

void CShaderObject::addTexture(const std::string& attachment, vk::DescriptorImageInfo& descriptor)
{
	mTextures[attachment] = descriptor;
}

void CShaderObject::addTexture(const std::string& attachment, std::shared_ptr<CImage>& pTexture)
{
	mTextures[attachment] = pTexture->getDescriptor();
}

void CShaderObject::addTexture(const std::string& attachment, std::weak_ptr<CImage>& pTexture)
{
	auto texture = pTexture.lock();
	mTextures[attachment] = texture->getDescriptor();
}

void CShaderObject::addTexture(const std::string& attachment, const std::unique_ptr<CImage>& pTexture)
{
    mTextures[attachment] = pTexture->getDescriptor();
}

vk::DescriptorImageInfo& CShaderObject::getTexture(const std::string& attachment)
{
	return mTextures[attachment];
}

std::unique_ptr<CHandler>& CShaderObject::getUniformBuffer(const std::string& name)
{
    auto& instance = vInstances.at(currentInstance);
    auto uniformBlock = instance->mBuffers.find(name);
    if (uniformBlock != instance->mBuffers.end())
        return uniformBlock->second;
    return pEmptyHandler;
}

std::map<std::string, std::unique_ptr<CHandler>>& CShaderObject::getUniformBuffers()
{
    return vInstances.at(currentInstance)->mBuffers;
}

std::unique_ptr<CDescriptorHandler>& CShaderObject::getDescriptorSet()
{
    return vInstances.at(currentInstance)->pDescriptorSet;
}