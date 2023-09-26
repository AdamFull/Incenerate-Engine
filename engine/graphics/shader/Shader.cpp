#include "Shader.h"

#include "APIHandle.h"
#include "APICompatibility.h"

using namespace engine::graphics;

CShader::CShader(CDevice* device)
{
    pDevice = device;
}

CShader::~CShader()
{
    for (auto& stage : vShaderModules)
        pDevice->destroy(&stage.module);

    pDevice = nullptr;
}

void CShader::addStage(const std::vector<uint32_t>& spirv, vk::ShaderStageFlagBits stage)
{
	CShaderReflect::addStage(spirv, stage);
    createShaderModule(spirv, stage);
}

void CShader::buildReflection()
{
    std::unordered_map<vk::DescriptorType, uint32_t> mDescriptorCounter;
    //Preparing descriptors for uniform/storage buffers
    for (const auto& [uniformBlockName, uniformBlock] : mUniformBlocks)
    {
        vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
        descriptorSetLayoutBinding.binding = uniformBlock.getBinding();
        descriptorSetLayoutBinding.descriptorType = uniformBlock.getDescriptorType();
        descriptorSetLayoutBinding.descriptorCount = 1;
        descriptorSetLayoutBinding.stageFlags = uniformBlock.getStageFlags();
        descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
        mDescriptorSetLayouts[uniformBlock.getSet()].emplace_back(descriptorSetLayoutBinding);
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
        mDescriptorSetLayouts[uniform.getSet()].emplace_back(descriptorSetLayoutBinding);
        mDescriptorCounter[uniform.getDescriptorType()]++;
    }

    for (auto& [descriptorType, count] : mDescriptorCounter)
        vDescriptorPools.emplace_back(vk::DescriptorPoolSize{ descriptorType, count * descriptorMultiplier });

    // Sort descriptors by binding.
    for (auto& [set, descriptors]:mDescriptorSetLayouts)
    {
        std::sort(descriptors.begin(), descriptors.end(),
            [](const vk::DescriptorSetLayoutBinding& l, const vk::DescriptorSetLayoutBinding& r) { return l.binding < r.binding; });
    }

    for (auto& module : vShaderModules)
    {
        auto& constants_data = mSpecConstantData[module.stage];
        for (auto& [name, constant] : mSpecializationConstants)
        {
            auto constant_stage_flags = constant.getStageFlags();
            if (constant_stage_flags & module.stage)
            {
                constants_data.entries.emplace_back();
                auto& entry = constants_data.entries.back();
                entry.constantID = constant.getConstantId();
                entry.size = constant.getSize();
                entry.offset = constant.getOffset();
            }
        }

        constants_data.info.mapEntryCount = static_cast<uint32_t>(constants_data.entries.size());
        constants_data.info.pMapEntries = constants_data.entries.data();
        constants_data.info.dataSize = static_cast<uint32_t>(vSpecializationBuffer.size());
        constants_data.info.pData = vSpecializationBuffer.data();
        module.pSpecializationInfo = &constants_data.info;
    }

    // Gets the last descriptors binding.
    //if (!vDescriptorSetLayouts.empty())
    //    lastDescriptorBinding = vDescriptorSetLayouts.back().binding;
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

CDevice* CShader::getDevice()
{
    return pDevice;
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
        log_cerror(APICompatibility::check(res), "Cannot create shader module.");

        vk::PipelineShaderStageCreateInfo shaderStageCI{};
        shaderStageCI.stage = stage;
        shaderStageCI.module = shaderModule;
        shaderStageCI.pName = "main";

        auto& debugUtils = pDevice->getDebugUtils();
        auto shader_id = (uint64_t)static_cast<VkShaderModule>(shaderModule);
        debugUtils->setObjectTag(vk::ObjectType::eShaderModule, shader_id, shader_id, spirv.data(), spirv.size());

        vShaderModules.emplace_back(std::move(shaderStageCI));
    }
    catch (vk::SystemError err)
    {
        log_error("Failed to create shader module!");
    }
}