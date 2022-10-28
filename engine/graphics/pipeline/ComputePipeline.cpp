#include "ComputePipeline.h"

#include "Device.h"
#include "shader/ShaderObject.h"

using namespace engine::graphics;

CComputePipeline::CComputePipeline(CDevice* device)
{
	pDevice = device;
}

void CComputePipeline::create(CShaderObject* pShader, vk::RenderPass& renderPass, uint32_t subpass)
{

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