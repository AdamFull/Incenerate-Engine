#include "Pipeline.h"

#include "APIHandle.h"
#include "shader/ShaderObject.h"

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

void CPipeline::create(CShaderObject* pShader)
{
    createDescriptorPool(pShader);
    createDescriptorSetLayout(pShader);
    createPipelineLayout(pShader);
}

void CPipeline::create(CShaderObject* pShader, vk::RenderPass& renderPass, uint32_t subpass)
{
    this->renderPass = renderPass;
    this->subpass = subpass;
    create(pShader);
}

void CPipeline::reCreate(CShaderObject* pShader, vk::RenderPass& renderPass, uint32_t subpass)
{
    this->renderPass = renderPass;
    this->subpass = subpass;
    createPipeline(pShader);
}

void CPipeline::bind(vk::CommandBuffer& commandBuffer)
{
    commandBuffer.bindPipeline(getBindPoint(), pipeline);
}

void CPipeline::createDescriptorSetLayout(CShaderObject* pShader)
{
    auto& shader = pShader->getShader();

    auto& descriptorSetLayouts = shader->getDescriptorSetLayouts();

    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayouts.data();

    vk::Result res = pDevice->create(descriptorSetLayoutCreateInfo, &descriptorSetLayout);
    log_cerror(VkHelper::check(res), "Cannot create descriptor set layout.");
}

void CPipeline::createDescriptorPool(CShaderObject* pShader)
{
    auto& shader = pShader->getShader();
    auto& descriptorPools = shader->getDescriptorPools();

    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    descriptorPoolCreateInfo.maxSets = 8192; // 16384;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPools.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPools.data();
    vk::Result res = pDevice->create(descriptorPoolCreateInfo, &descriptorPool);
    log_cerror(VkHelper::check(res), "Cannot create descriptor pool.");
}

void CPipeline::createPipelineLayout(CShaderObject* pShader)
{
    auto& shader = pShader->getShader();
    auto pushConstantRanges = shader->getPushConstantRanges();

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
    pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();
    vk::Result res = pDevice->create(pipelineLayoutCreateInfo, &pipelineLayout);
    log_cerror(VkHelper::check(res), "Cannot create pipeline layout.");
}