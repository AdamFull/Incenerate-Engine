#include "GraphicsPipeline.h"

#include "Device.h"
#include "shader/ShaderObject.h"
#include "APIStructures.h"

using namespace engine::graphics;

CGraphicsPipeline::CGraphicsPipeline(CDevice* device)
{
	pDevice = device;
}

void CGraphicsPipeline::create(CShaderObject* pShader, vk::RenderPass& renderPass, uint32_t subpass)
{
	CPipeline::create(pShader, renderPass, subpass);
	createPipeline(pShader);
}

void CGraphicsPipeline::createPipeline(CShaderObject* pShader)
{
    auto& shader = pShader->getShader();
    auto vertexfree = pShader->isVertexFree();
    auto culling = pShader->getCullMode();
    auto frontface = pShader->getFrontFace();
    auto depthTest = pShader->getDepthTestFlag();
    auto dynamicStateEnables = pShader->getDynamicStateEnables();
    auto enableTesselation = pShader->getTesselationFlag();
    auto topology = pShader->getPrimitiveTopology();

    auto attributeDescription = FVertex::getAttributeDescriptions();
    auto bindingDescription = FVertex::getBindingDescription();

    vk::PipelineVertexInputStateCreateInfo vertexInputCI{};
    vertexInputCI.vertexBindingDescriptionCount = 0;
    vertexInputCI.vertexAttributeDescriptionCount = 0;
    if (!vertexfree)
    {
        vertexInputCI.vertexBindingDescriptionCount = attributeDescription.size() > 0 ? 1 : 0;
        vertexInputCI.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
        vertexInputCI.pVertexBindingDescriptions = &bindingDescription;
        vertexInputCI.pVertexAttributeDescriptions = attributeDescription.data();
    }

    auto attachmentCount = 1;
    bool isDepthOnly = attachmentCount == 0;

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.topology = topology;
    inputAssembly.flags = vk::PipelineInputAssemblyStateCreateFlags{};
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.depthClampEnable = isDepthOnly;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = culling;
    rasterizer.frontFace = frontface;
    rasterizer.depthBiasEnable = VK_FALSE;

    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments;
    for (uint32_t i = 0; i < attachmentCount; i++)
    {
        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
        colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
        colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
        colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

        colorBlendAttachments.emplace_back(colorBlendAttachment);
    }

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = vk::LogicOp::eCopy;
    colorBlending.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
    colorBlending.pAttachments = colorBlendAttachments.data();

    vk::PipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.depthTestEnable = depthTest;
    depthStencil.depthWriteEnable = depthTest;
    depthStencil.depthCompareOp = vk::CompareOp::eLessOrEqual;
    depthStencil.back.compareOp = vk::CompareOp::eAlways;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
    dynamicStateInfo.pDynamicStates = dynamicStateEnables.data();
    dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
    dynamicStateInfo.flags = vk::PipelineDynamicStateCreateFlags{};

    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    vk::PipelineTessellationStateCreateInfo tessellationState{};
    tessellationState.patchControlPoints = 3;

    auto shaderStages = shader->getStageCreateInfo();

    vk::GraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.stageCount = shaderStages.size();
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputCI;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = subpass;
    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.pDynamicState = &dynamicStateInfo;

    if (enableTesselation)
        pipelineInfo.pTessellationState = &tessellationState;


    auto result = pDevice->create(pipelineInfo, &pipeline);
    assert(pipeline && "Failed creating pipeline.");
}