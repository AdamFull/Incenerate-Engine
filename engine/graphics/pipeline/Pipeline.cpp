#include "Pipeline.h"

#include "Device.h"

using namespace engine::graphics;

CPipeline::CPipeline(CDevice* device)
{
	pDevice = device;
}

CPipeline::~CPipeline()
{
    if (descriptorSetLayout)
        pDevice->destroy(&descriptorSetLayout);
    if (descriptorPool)
        pDevice->destroy(&descriptorPool);
    if (pipeline)
        pDevice->destroy(&pipeline);
    if (pipelineLayout)
        pDevice->destroy(&pipelineLayout);

    pDevice = nullptr;
}

void CPipeline::create(const std::unique_ptr<CShaderObject>& pShader)
{
    createDescriptorPool(pShader);
    createDescriptorSetLayout(pShader);
    createPipelineLayout(pShader);
}

void CPipeline::create(const std::unique_ptr<CShaderObject>& pShader, vk::RenderPass& renderPass, uint32_t subpass)
{
    this->renderPass = renderPass;
    this->subpass = subpass;
    create(pShader);
}

void CPipeline::reCreate(const std::unique_ptr<CShaderObject>& pShader, vk::RenderPass& renderPass, uint32_t subpass)
{
    this->renderPass = renderPass;
    this->subpass = subpass;
    createPipeline(pShader);
}

void CPipeline::bind(vk::CommandBuffer& commandBuffer)
{
    commandBuffer.bindPipeline(getBindPoint(), pipeline);
}

void CPipeline::createDescriptorSetLayout(const std::unique_ptr<CShaderObject>& pShader)
{
    auto& shader = pShader->getShader();

    auto& descriptorSetLayouts = shader->getDescriptorSetLayouts();

    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayouts.data();

    vk::Result res = pDevice->create(descriptorSetLayoutCreateInfo, &descriptorSetLayout);
    assert(res == vk::Result::eSuccess && "Cannot create descriptor set layout.");
}

void CPipeline::createDescriptorPool(const std::unique_ptr<CShaderObject>& pShader)
{
    auto& shader = pShader->getShader();
    auto& descriptorPools = shader->getDescriptorPools();

    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    descriptorPoolCreateInfo.maxSets = 8192; // 16384;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPools.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPools.data();
    vk::Result res = pDevice->create(descriptorPoolCreateInfo, &descriptorPool);
    assert(res == vk::Result::eSuccess && "Cannot create descriptor pool.");
}

void CPipeline::createPipelineLayout(const std::unique_ptr<CShaderObject>& pShader)
{
    auto& shader = pShader->getShader();
    auto pushConstantRanges = shader->getPushConstantRanges();

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
    pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();
    vk::Result res = pDevice->create(pipelineLayoutCreateInfo, &pipelineLayout);
    assert(res == vk::Result::eSuccess && "Cannot create pipeline layout.");
}