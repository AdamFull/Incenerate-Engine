#include "ComputePipeline.h"

#include "Device.h"

using namespace engine::graphics;

CComputePipeline::CComputePipeline(CDevice* device)
{
	pDevice = device;
}

void CComputePipeline::create(const std::unique_ptr<CShaderObject>& pShader, vk::RenderPass& renderPass, uint32_t subpass)
{

}

void CComputePipeline::createPipeline(const std::unique_ptr<CShaderObject>& pShader)
{
    auto& shader = pShader->getShader();
    auto shaderStages = shader->getStageCreateInfo();

    vk::ComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.stage = shaderStages.front();
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.basePipelineIndex = -1;

    vk::Result res = pDevice->create(pipelineInfo, &pipeline);
    assert(res == vk::Result::eSuccess && "Failed creating pipeline.");
}