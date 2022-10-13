#include "ShaderObject.h"

#include "image/Image.h"

using namespace engine::graphics;

const std::unique_ptr<CShader>& CShaderObject::getShader()
{
	return pShader;
}

void CShaderObject::create()
{
    if (!bIsCreated)
    {

        bIsCreated = true;
    }
}

void CShaderObject::reCreate()
{
    if (!bIsReCreated)
    {
        bIsReCreated = true;
    }
}

void CShaderObject::update()
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
}

void CShaderObject::bind(vk::CommandBuffer& commandBuffer)
{
    auto& pDescriptorSet = getDescriptorSet();
    pDescriptorSet->bind(commandBuffer);
    pPipeline->bind(commandBuffer);
    bIsReCreated = false;
    currentInstance = (currentInstance + 1) % instances;
}

void CShaderObject::addTexture(const std::string& attachment, vk::DescriptorImageInfo& descriptor)
{
	mTextures[attachment] = descriptor;
}

void CShaderObject::addTexture(const std::string& attachment, std::shared_ptr<CImage>& pTexture)
{
	mTextures[attachment] = pTexture->getDescriptor();
}

void CShaderObject::addTexture(const std::string& attachment, utl::weak_ptr<CImage>& pTexture)
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

std::map<std::string, utl::scope_ptr<CHandler>>& CShaderObject::getUniformBuffers()
{
    return vInstances.at(currentInstance)->mBuffers;
}

utl::scope_ptr<CDescriptorHandler>& CShaderObject::getDescriptorSet()
{
    return vInstances.at(currentInstance)->pDescriptorSet;
}