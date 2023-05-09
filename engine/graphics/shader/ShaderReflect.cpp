#include "ShaderReflect.h"

using namespace engine::graphics;

void CShaderReflect::addStage(const std::vector<uint32_t>& spirv, vk::ShaderStageFlagBits stage)
{
	processReflection(spirv, stage);
}

const CUniform* CShaderReflect::getUniform(const std::string& name) const
{
    if (const auto& it = mUniforms.find(name); it != mUniforms.end())
        return &it->second;
    return nullptr;
}

const CUniformBlock* CShaderReflect::getUniformBlock(const std::string& name) const
{
	if (const auto& it = mUniformBlocks.find(name); it != mUniformBlocks.end())
        return &it->second;
    return nullptr;
}

const CPushConstBlock* CShaderReflect::getPushBlock(const std::string& name) const
{
	if (const auto& it = mPushBlocks.find(name); it != mPushBlocks.end())
        return &it->second;
	return nullptr;
}

void CShaderReflect::processReflection(const std::vector<uint32_t>& spirv, vk::ShaderStageFlagBits stageFlag)
{
    spirv_cross::CompilerGLSL compiler(spirv);
    auto active = compiler.get_active_interface_variables();
    auto resources = compiler.get_shader_resources(active);
    compiler.set_enabled_interface_variables(std::move(active));

    auto& execution_modes = compiler.get_execution_mode_bitset();
    execution_modes.for_each_bit([&](uint32_t i)
        {
            auto mode = static_cast<spv::ExecutionMode>(i);
            switch (mode)
            {
            case spv::ExecutionMode::ExecutionModeInvocations:
            {
                executionModeInvocations = compiler.get_execution_mode_argument(mode, 0);
            } break;
            case spv::ExecutionMode::ExecutionModeLocalSize: 
            {
                std::array<std::optional<uint32_t>, 3> sizes;
                for (uint32_t dim = 0; dim < 3; ++dim)
                    sizes[dim] = compiler.get_execution_mode_argument(mode, dim);
                localSizes.emplace_back(sizes);
            } break;
            case spv::ExecutionMode::ExecutionModeOutputVertices: {
                executionModeOutputVertices = compiler.get_execution_mode_argument(mode, 0);
            } break;
            }
        });

    //Parsing uniform buffers
    for (const auto& res : resources.uniform_buffers)
    {
        //Looking for the block if already exists and put stage flag
        auto it = mUniformBlocks.find(res.name);
        if (it != mUniformBlocks.end())
            it->second.stageFlags |= stageFlag;
        else
            mUniformBlocks.emplace(res.name, buildUniformBlock(&compiler, res, stageFlag, vk::DescriptorType::eUniformBuffer));
    }

    for (const auto& res : resources.storage_buffers)
    {
        auto it = mUniformBlocks.find(res.name);
        if (it != mUniformBlocks.end())
            it->second.stageFlags |= stageFlag;
        else
            mUniformBlocks.emplace(res.name, buildUniformBlock(&compiler, res, stageFlag, vk::DescriptorType::eStorageBuffer));
    }

    //check comp.get_active_buffer_ranges(res.push_constant_buffers[0].id);
    for (const auto& res : resources.push_constant_buffers)
    {
        auto it = mPushBlocks.find(res.name);
        if (it != mPushBlocks.end())
            it->second.stageFlags |= stageFlag;
        else
            mPushBlocks.emplace(res.name, buildPushBlock(&compiler, res, stageFlag));
    }
    
    for (const auto& res : resources.sampled_images)
    {
        auto it = mUniforms.find(res.name);
        if (it != mUniforms.end())
            it->second.stageFlags |= stageFlag;
        else
        {
            // TODO: bad code. Here i should ignore "set = 2"
            if(res.name != "textures")
                mUniforms.emplace(res.name, buildUnifrom(&compiler, res, stageFlag, vk::DescriptorType::eCombinedImageSampler));
        }
    }

    for (const auto& res : resources.separate_images)
    {
        auto it = mUniforms.find(res.name);
        if (it != mUniforms.end())
            it->second.stageFlags |= stageFlag;
        else
            mUniforms.emplace(res.name, buildUnifrom(&compiler, res, stageFlag, vk::DescriptorType::eSampledImage));
    }

    for (const auto& res : resources.separate_samplers)
    {
        auto it = mUniforms.find(res.name);
        if (it != mUniforms.end())
            it->second.stageFlags |= stageFlag;
        else
            mUniforms.emplace(res.name, buildUnifrom(&compiler, res, stageFlag, vk::DescriptorType::eSampler));
    }

    for (const auto& res : resources.storage_images)
    {
        auto it = mUniforms.find(res.name);
        if (it != mUniforms.end())
            it->second.stageFlags |= stageFlag;
        else
            mUniforms.emplace(res.name, buildUnifrom(&compiler, res, stageFlag, vk::DescriptorType::eStorageImage));
    }

    for (const auto& res : resources.subpass_inputs)
    {
        auto it = mUniforms.find(res.name);
        if (it != mUniforms.end())
            it->second.stageFlags |= stageFlag;
        else
            mUniforms.emplace(res.name, buildUnifrom(&compiler, res, stageFlag, vk::DescriptorType::eInputAttachment));
    }
}

CUniformBlock CShaderReflect::buildUniformBlock(spirv_cross::CompilerGLSL* compiler, const spirv_cross::Resource& res, vk::ShaderStageFlagBits stageFlag, vk::DescriptorType descriptorType)
{
	const auto& type = compiler->get_type(res.type_id);
	auto& decoration = compiler->get_decoration_bitset(type.self);
	unsigned member_count = type.member_types.size();
	CUniformBlock uniformBlock{};
	uniformBlock.set = compiler->get_decoration(res.id, spv::DecorationDescriptorSet);
	uniformBlock.binding = compiler->get_decoration(res.id, spv::DecorationBinding);

	for (uint32_t i = 0; i < member_count; i++)
	{
		size_t array_stride{ 0 }, matrix_stride{ 0 };
		auto& member_type = compiler->get_type(type.member_types[i]);
		CUniform uniform{};
		//compiler.get_member_decoration(, i, spv::DecorationDescriptorSet);
		uniform.size = compiler->get_declared_struct_member_size(type, i);
		uniform.offset = compiler->type_struct_member_offset(type, i);
		if (!member_type.array.empty() && uniform.size == 0)
			array_stride = compiler->type_struct_member_array_stride(type, i);

		if (member_type.columns > 1)
			matrix_stride = compiler->type_struct_member_matrix_stride(type, i);

		uniform.stageFlags = stageFlag;
		uniformBlock.mUniforms.emplace(compiler->get_member_name(type.self, i), uniform);
	}

	uniformBlock.descriptorType = descriptorType;
	uniformBlock.size = compiler->get_declared_struct_size(type);
	return uniformBlock;
}

CUniform CShaderReflect::buildUnifrom(spirv_cross::CompilerGLSL* compiler, const spirv_cross::Resource& res, vk::ShaderStageFlagBits stageFlag, vk::DescriptorType descriptorType)
{
	CUniform uniform{};
	uniform.set = compiler->get_decoration(res.id, spv::DecorationDescriptorSet);
	uniform.binding = compiler->get_decoration(res.id, spv::DecorationBinding);
	uniform.descriptorType = descriptorType;
	uniform.stageFlags = stageFlag;
	return uniform;
}

CPushConstBlock CShaderReflect::buildPushBlock(spirv_cross::CompilerGLSL* compiler, const spirv_cross::Resource& res, vk::ShaderStageFlagBits stageFlag)
{
	const auto& type = compiler->get_type(res.type_id);
	unsigned member_count = type.member_types.size();
	CPushConstBlock pushBlock{};

	for (uint32_t i = 0; i < member_count; i++)
	{
		auto& member_type = compiler->get_type(type.member_types[i]);
		CUniform uniform{};
		uniform.size = compiler->get_declared_struct_member_size(type, i);
		uniform.offset = compiler->type_struct_member_offset(type, i);
		if (!member_type.array.empty()) { size_t array_stride = compiler->type_struct_member_array_stride(type, i); }
		if (member_type.columns > 1) { size_t matrix_stride = compiler->type_struct_member_matrix_stride(type, i); }
		uniform.stageFlags = stageFlag;
		pushBlock.mUniforms.emplace(compiler->get_member_name(type.self, i), uniform);
	}

	pushBlock.size = compiler->get_declared_struct_size(type);
	return pushBlock;
}