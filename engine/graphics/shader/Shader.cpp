#include "Shader.h"

#include "Device.h"

using namespace engine::graphics;

CShader::CShader(CDevice* device)
{
    pDevice = device;
}

CShader::~CShader()
{
    for (auto& stage : vShaderModules)
        pDevice->destroy(&stage.module);
}

void CShader::addStage(const std::vector<uint32_t>& spirv, vk::ShaderStageFlagBits stage)
{
	CShaderReflect::addStage(spirv, stage);
    createShaderModule(spirv, stage);
}

void CShader::buildReflection()
{
    std::map<vk::DescriptorType, uint32_t> mDescriptorCounter;
    //Preparing descriptors for uniform/storage buffers
    for (const auto& [uniformBlockName, uniformBlock] : mUniformBlocks)
    {
        vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
        descriptorSetLayoutBinding.binding = uniformBlock.getBinding();
        descriptorSetLayoutBinding.descriptorType = uniformBlock.getDescriptorType();
        descriptorSetLayoutBinding.descriptorCount = 1;
        descriptorSetLayoutBinding.stageFlags = uniformBlock.getStageFlags();
        descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
        vDescriptorSetLayouts.emplace_back(descriptorSetLayoutBinding);
        mDescriptorCounter[uniformBlock.getDescriptorType()]++;
    }

    for (const auto& [uniformName, uniform] : mUniforms)
    {
        vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
        descriptorSetLayoutBinding.binding = uniform.getBinding();
        descriptorSetLayoutBinding.descriptorType = uniform.getDescriptorType();
        descriptorSetLayoutBinding.descriptorCount = 1;
        descriptorSetLayoutBinding.stageFlags = uniform.getStageFlags();
        descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
        vDescriptorSetLayouts.emplace_back(descriptorSetLayoutBinding);
        mDescriptorCounter[uniform.getDescriptorType()]++;
    }

    for (auto& [descriptorType, count] : mDescriptorCounter)
        vDescriptorPools.emplace_back(vk::DescriptorPoolSize{ descriptorType, count * descriptorMultiplier });

    // Sort descriptors by binding.
    std::sort(vDescriptorSetLayouts.begin(), vDescriptorSetLayouts.end(),
        [](const vk::DescriptorSetLayoutBinding& l, const vk::DescriptorSetLayoutBinding& r)
        {
            return l.binding < r.binding;
        });

    // Gets the last descriptors binding.
    if (!vDescriptorSetLayouts.empty())
        lastDescriptorBinding = vDescriptorSetLayouts.back().binding;
}

std::vector<vk::PushConstantRange> CShader::getPushConstantRanges() const
{
    std::vector<vk::PushConstantRange> pushConstantRanges;
    uint32_t currentOffset = 0;

    for (const auto& [pushBlockName, pushBlock] : mPushBlocks)
    {
        vk::PushConstantRange pushConstantRange = {};
        pushConstantRange.stageFlags = pushBlock.getStageFlags();
        pushConstantRange.offset = currentOffset;
        pushConstantRange.size = static_cast<uint32_t>(pushBlock.getSize());
        pushConstantRanges.emplace_back(pushConstantRange);
        currentOffset += pushConstantRange.size;
    }
    return pushConstantRanges;
}

void CShader::createShaderModule(const std::vector<uint32_t>& spirv, vk::ShaderStageFlagBits stage)
{
    try
    {
        vk::ShaderModuleCreateInfo shaderCI{};
        shaderCI.codeSize = spirv.size() * sizeof(uint32_t);
        shaderCI.pCode = spirv.data();

        vk::ShaderModule shaderModule{ VK_NULL_HANDLE };
        vk::Result res = pDevice->create(shaderCI, &shaderModule);
        assert(res == vk::Result::eSuccess && "Cannot create shader module.");

        vk::PipelineShaderStageCreateInfo shaderStageCI{};
        shaderStageCI.stage = stage;
        shaderStageCI.module = shaderModule;
        shaderStageCI.pName = "main";

        vShaderModules.emplace_back(std::move(shaderStageCI));
    }
    catch (vk::SystemError err)
    {
        throw std::runtime_error("Failed to create shader module!");
    }
}