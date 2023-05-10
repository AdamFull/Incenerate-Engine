#include "Pipeline.h"

#include "APIHandle.h"
#include "APICompatibility.h"
#include "shader/ShaderObject.h"

#include <SessionStorage.hpp>

using namespace engine::graphics;

CPipeline::CPipeline(CDevice* device)
{
    pDevice = device;
}

CPipeline::~CPipeline()
{
    for(auto& [set, layout] : mDescriptorSetLayouts)
        pDevice->destroy(&layout);
    if (descriptorPool)
        pDevice->destroy(&descriptorPool);
    for(auto& pipeline : vPipelines)
        pDevice->destroy(&pipeline);
    if (pipelineLayout)
        pDevice->destroy(&pipelineLayout);

    pDevice = nullptr;
}

void CPipeline::create(CShaderObject* pShader)
{
    bBindlessFeature = CSessionStorage::getInstance()->get<bool>(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
    bCanBeBindless = pShader->isUsesBindlessTextures();
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

void CPipeline::bind(vk::CommandBuffer& commandBuffer, uint32_t index)
{
    commandBuffer.bindPipeline(getBindPoint(), vPipelines[index]);
}

CDevice* CPipeline::getDevice()
{
    return pDevice;
}

void CPipeline::createDescriptorSetLayout(CShaderObject* pShader)
{
    auto& shader = pShader->getShader();

    auto& mDescriptorSetLayoutsCI = shader->getDescriptorSetLayouts();

    for (auto& [set, descriptorSetLayouts] : mDescriptorSetLayoutsCI)
    {
        vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
        descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayouts.data();

        vk::DescriptorSetLayout descriptorSetLayout;
        vk::Result res = pDevice->create(descriptorSetLayoutCreateInfo, &descriptorSetLayout);
        log_cerror(APICompatibility::check(res), "Cannot create descriptor set layout.");

        mDescriptorSetLayouts.emplace(set, descriptorSetLayout);
    }
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
    log_cerror(APICompatibility::check(res), "Cannot create descriptor pool.");
}

void CPipeline::createPipelineLayout(CShaderObject* pShader)
{
    auto& shader = pShader->getShader();
    auto pushConstantRanges = shader->getPushConstantRanges();

    std::vector<vk::DescriptorSetLayout> vLayouts{};
    for (auto& [set, layout] : mDescriptorSetLayouts)
        vLayouts.emplace_back(layout);

    // TODO: bad practice
    if (bBindlessFeature && bCanBeBindless)
    {
        auto graphics = pDevice->getAPI();
        auto& bindlessDescriptor = graphics->getBindlessDescriptor();
        vLayouts.emplace_back(bindlessDescriptor->getDescriptorSetLayout());
    }

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.setLayoutCount = vLayouts.size();
    pipelineLayoutCreateInfo.pSetLayouts = vLayouts.data();
    pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
    pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();
    vk::Result res = pDevice->create(pipelineLayoutCreateInfo, &pipelineLayout);
    log_cerror(APICompatibility::check(res), "Cannot create pipeline layout.");
}