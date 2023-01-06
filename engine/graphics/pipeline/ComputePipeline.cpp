#include "ComputePipeline.h"

#include "APIHandle.h"
#include "shader/ShaderObject.h"

using namespace engine::graphics;

CComputePipeline::CComputePipeline(CDevice* device)
{
    pDevice = device;
}

void CComputePipeline::create(CShaderObject* pShader)
{
    CPipeline::create(pShader);
    createPipeline(pShader);
}

void CComputePipeline::createPipeline(CShaderObject* pShader)
{
    auto& shader = pShader->getShader();
    auto shaderStages = shader->getStageCreateInfo();

    vk::ComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.stage = shaderStages.front();
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.basePipelineIndex = -1;

    vk::Result res = pDevice->create(pipelineInfo, &pipeline);
    log_cerror(VkHelper::check(res), "Failed creating pipeline.");
}