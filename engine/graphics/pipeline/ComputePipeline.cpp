#include "ComputePipeline.h"

#include "APIHandle.h"
#include "APICompatibility.h"
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

    // Maybe set base pipeline here? If exists exactly.
    for (uint32_t i = 0; i < shaderStages.size(); ++i)
    {
        vk::Pipeline pipeline{ nullptr };

        vk::ComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.stage = shaderStages[i];
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.basePipelineHandle = nullptr;
        pipelineInfo.basePipelineIndex = -1;

        vk::Result res = pDevice->create(pipelineInfo, &pipeline);
        log_cerror(APICompatibility::check(res), "Failed creating compute pipeline.");
        vPipelines.emplace_back(pipeline);
    }
}