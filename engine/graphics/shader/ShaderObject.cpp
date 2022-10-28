#include "ShaderObject.h"

#include "Device.h"
#include "image/Image.h"
#include "buffers/CommandBuffer.h"
#include "pipeline/GraphicsPipeline.h"
#include "pipeline/ComputePipeline.h"
#include "handlers/StorageHandler.h"
#include "handlers/UniformHandler.h"

using namespace engine::graphics;

CShaderObject::CShaderObject(CDevice* device)
{
    pDevice = device;

    pShader = std::make_unique<CShader>(pDevice);
    pFramebuffer = std::make_unique<CFramebuffer>(pDevice);
    pVBO = std::make_unique<CVertexBufferObject>(pDevice);
}

const std::unique_ptr<CShader>& CShaderObject::getShader()
{
	return pShader;
}

void CShaderObject::create()
{
    if (!bIsCreated)
    {
        //Creating framebuffer
        pFramebuffer->setRenderArea(vk::Offset2D{0, 0}, pDevice->getExtent());
        pFramebuffer->addImage("present_khr", pDevice->getImageFormat(), vk::ImageUsageFlagBits::eColorAttachment);
        pFramebuffer->addImage("depth_tex", pDevice->getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment);
        pFramebuffer->addOutputReference(0U, "present_khr");
        pFramebuffer->addDescription(0U, "depth_tex");

        pFramebuffer->addSubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eFragmentShader,
            vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
            vk::AccessFlagBits::eShaderRead, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
        pFramebuffer->addSubpassDependency(0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
            vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite, vk::AccessFlagBits::eColorAttachmentWrite);

        pFramebuffer->setFlipViewport(true);
        pFramebuffer->create();

        //Creating pipeline
        switch (programCI.bindPoint)
        {
        case vk::PipelineBindPoint::eGraphics: {
            pPipeline = std::make_unique<CGraphicsPipeline>(pDevice);
            pPipeline->create(this, pFramebuffer->getRenderPass(), 0);
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

        bIsCreated = true;
    }
}

void CShaderObject::reCreate()
{
    if (!bIsReCreated)
    {
        pFramebuffer->setRenderArea(vk::Offset2D{ 0, 0 }, pDevice->getExtent());
        pFramebuffer->reCreate();
        bIsReCreated = true;
    }
}

void CShaderObject::setRenderFunc(utl::function<void(CShaderObject*, vk::CommandBuffer&)>&& rf)
{
    pRenderFunc = std::move(rf);
}

void CShaderObject::render(vk::CommandBuffer& commandBuffer)
{
    pFramebuffer->begin(commandBuffer);

    if (pRenderFunc)
        pRenderFunc(this, commandBuffer);

    bind(commandBuffer);
    pFramebuffer->end(commandBuffer);
}

void CShaderObject::dispatch(size_t size)
{
    auto cmdBuf = CCommandBuffer(pDevice);
    cmdBuf.create(true, vk::QueueFlagBits::eCompute);
    auto& commandBuffer = cmdBuf.getCommandBuffer();

    if (pRenderFunc)
        pRenderFunc(this, commandBuffer);

    bind(commandBuffer, false);

    auto groupCountX = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*pShader->getLocalSizes()[0])));
    auto groupCountY = static_cast<uint32_t>(std::ceil(static_cast<float>(size) / static_cast<float>(*pShader->getLocalSizes()[1])));
    commandBuffer.dispatch(groupCountX, groupCountY, 1);
    cmdBuf.submitIdle();
}

void CShaderObject::bind(vk::CommandBuffer& commandBuffer, bool drawcall)
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

    if (drawcall)
        pVBO->bind(commandBuffer);
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