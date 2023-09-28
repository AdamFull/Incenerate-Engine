#include "GraphicsPipeline.h"

#include "APIHandle.h"
#include "shader/ShaderObject.h"
#include "APIStructures.h"
#include "Engine.h"

#include <vulkan/vulkan_format_traits.hpp>

using namespace engine::graphics;

CGraphicsPipeline::CGraphicsPipeline(CDevice* device)
{
    pDevice = device;
}

void CGraphicsPipeline::create(CShaderObject* pShader, vk::RenderPass& renderPass, const FShaderCreateInfo& specials)
{
	CPipeline::create(pShader, renderPass, specials);
	createPipeline(pShader, specials);
}

// TODO: create structure FPipelineCreateInfo
void CGraphicsPipeline::createPipeline(CShaderObject* pShader, const FShaderCreateInfo& specials)
{
    auto graphics = pDevice->getAPI();
    auto& debugUtils = pDevice->getDebugUtils();
    auto& shader = pShader->getShader();

    vk::PipelineVertexInputStateCreateInfo vertexInputCI{};
    vertexInputCI.vertexBindingDescriptionCount = 0;
    vertexInputCI.vertexAttributeDescriptionCount = 0;

    std::vector<vk::VertexInputAttributeDescription> attributeDescription;
    std::vector<vk::VertexInputBindingDescription> bindingDescriptions;

    if (specials.vertex_type != EVertexType::eNone)
    {
        if (specials.vertex_type == EVertexType::eDefault)
        {
            attributeDescription = FVertex::getAttributeDescriptions();
            bindingDescriptions = FVertex::getBindingDescription();
        }
        else if (specials.vertex_type == EVertexType::eSmall)
        {
            attributeDescription = FSimpleVertex::getAttributeDescriptions();
            bindingDescriptions = FSimpleVertex::getBindingDescription();
        }
        
        vertexInputCI.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
        vertexInputCI.pVertexAttributeDescriptions = attributeDescription.data();
        vertexInputCI.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
        vertexInputCI.pVertexBindingDescriptions = bindingDescriptions.data();
    }

    uint32_t colorAttachmentCount{ 0 };
    auto depthformat = graphics->getDevice()->getDepthFormat();
    auto& framebuffer = graphics->getFramebuffer(specials.pipeline_stage);
    auto descriptions = framebuffer->getAttachmentDescriptions();

    std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments;
    for (auto& desc : descriptions)
    {
        if (desc.format == depthformat)
            continue;

        bool hasAlpha = specials.alpha_blend == EAlphaMode::EBLEND;

        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.blendEnable = hasAlpha;

        colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
        colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
        colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
        colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
        colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

        colorBlendAttachments.emplace_back(colorBlendAttachment);
        colorAttachmentCount++;
    }
    
    bool isDepthOnly = colorAttachmentCount == 0;

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.topology = false ? vk::PrimitiveTopology::ePatchList : specials.primitive_topology;
    inputAssembly.flags = vk::PipelineInputAssemblyStateCreateFlags{};
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.depthClampEnable = isDepthOnly;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = specials.double_sided ? vk::CullModeFlagBits::eNone : specials.cull_mode;
    rasterizer.frontFace = specials.front_face;
    rasterizer.depthBiasEnable = VK_FALSE;

    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = vk::LogicOp::eCopy;
    colorBlending.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
    colorBlending.pAttachments = colorBlendAttachments.data();

    vk::PipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.depthTestEnable = specials.depth_test;
    depthStencil.depthWriteEnable = specials.depth_test;
    depthStencil.depthCompareOp = vk::CompareOp::eLessOrEqual;
    depthStencil.back.compareOp = vk::CompareOp::eAlways;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    std::vector<vk::DynamicState> dynamic_states{ vk::DynamicState::eViewport, vk::DynamicState::eScissor };

    vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
    dynamicStateInfo.pDynamicStates = dynamic_states.data();
    dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
    dynamicStateInfo.flags = vk::PipelineDynamicStateCreateFlags{};

    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    vk::PipelineTessellationStateCreateInfo tessellationState{};
    tessellationState.patchControlPoints = false ? shader->getControlPoints() : 0;

    auto& shaderStages = shader->getStageCreateInfo();

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

    if (false)
        pipelineInfo.pTessellationState = &tessellationState;

    vk::Pipeline pipeline{ nullptr };
    auto result = pDevice->create(pipelineInfo, &pipeline);
    log_cerror(pipeline, "Failed creating pipeline.");

    //debugUtils->setObjectName(vk::ObjectType::ePipeline, (uint64_t)static_cast<VkPipeline>(pipeline), );

    vPipelines.emplace_back(pipeline);
}