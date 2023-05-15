#include "ShaderLoader.h"

#include "Engine.h"
#include "graphics/rendering/material/Material.h"
#include "filesystem/vfs_helper.h"
#include <interface/filesystem/VirtualFileSystemInterface.h>
#include "pipeline/GraphicsPipeline.h"
#include "pipeline/ComputePipeline.h"

#include <SessionStorage.hpp>

constexpr const char* shader_config_path = "/embed/shaders.json";

using namespace engine::graphics;
using namespace engine::filesystem;

size_t spirv_hash(const std::vector<uint32_t>& spirv)
{
	auto seed = spirv.size();
	for (auto x : spirv)
	{
		x = ((x >> 16) ^ x) * 0x45d9f3b;
		x = ((x >> 16) ^ x) * 0x45d9f3b;
		x = (x >> 16) ^ x;
		seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	return seed;
}

size_t params_hash(const FPipelineParams& params) 
{
	std::stringstream ss;
	
	ss <<
		static_cast<uint32_t>(params.alphaMode) <<
		static_cast<uint32_t>(params.bindPoint) <<
		static_cast<uint32_t>(params.cullMode) <<
		static_cast<uint32_t>(params.frontFace) <<
		static_cast<uint32_t>(params.primitiveTopology) <<
		static_cast<uint32_t>(params.vertexType) <<
		params.depthTest <<
		params.doubleSided <<
		params.enableTesselation <<
		params.useBindlessTextures <<
		params.vertexFree <<
		params.renderStage;

	for (auto& dynamicState : params.dynamicStates)
		ss << static_cast<uint32_t>(dynamicState);

	return std::hash<std::string>()(ss.str());
}

std::string shader_object_hash(const std::vector<uint32_t>& spirv, const FPipelineParams& params)
{
	return std::to_string(spirv_hash(spirv)) + std::to_string(params_hash(params));
}

CShaderLoader::CShaderLoader(CDevice* device, IVirtualFileSystemInterface* vfs_ptr) :
	m_pDevice(device), m_pVFS(vfs_ptr)
{
	m_pCompiler = std::make_unique<CShaderCompiler>(m_pVFS);

	bEditorMode = CSessionStorage::getInstance()->get<bool>("editor_mode");
	bBindlessFeature = CSessionStorage::getInstance()->get<bool>(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
}

CShaderLoader::~CShaderLoader()
{
	m_pCompiler = nullptr;
	m_pDevice = nullptr;
}

void CShaderLoader::create()
{
	m_pVFS->readJson(shader_config_path, programCI);
}

std::string CShaderLoader::getShaderCreateInfo(const std::string& shaderName, size_t mat_id, std::vector<std::optional<FCachedShader>>& shaderCode, FPipelineParams& pipelineParams)
{
	auto* graphics = m_pDevice->getAPI();
	FShaderSpecials specials;

	if (mat_id != invalid_index)
	{
		auto& pMaterial = graphics->getMaterial(mat_id);
		auto& params = pMaterial->getParameters();
		specials.usages = pMaterial->getUsageCount();
		specials.doubleSided = params.doubleSided;
		specials.alphaBlend = params.alphaMode;

		for (auto& definition : params.vCompileDefinitions)
			specials.defines.emplace(definition, "");
	}

	return getShaderCreateInfo(shaderName, specials, shaderCode, pipelineParams);
}

std::string CShaderLoader::getShaderCreateInfo(const std::string& shaderName, const FShaderSpecials& specials, std::vector<std::optional<FCachedShader>>& shaderCode, FPipelineParams& pipelineParams)
{
	auto* graphics = m_pDevice->getAPI();

	auto it = programCI.find(shaderName);
	if (it != programCI.end())
	{
		std::vector<uint32_t> vShaderCode{};

		std::stringstream defineBlock;
		for (auto& [dname, value] : specials.defines)
			defineBlock << "#define " << dname << " " << value << '\n';

		for (const auto& [defineName, defineValue] : it->second.defines)
			defineBlock << "#define " << defineName << " " << defineValue << '\n';

		if (bEditorMode)
			defineBlock << "#define " << "EDITOR_MODE" << "\n";

		if (bBindlessFeature)
			defineBlock << "#define " << "BINDLESS_TEXTURES" << "\n";

		auto stages = it->second.stages;
		if (!it->second.tesselation)
		{
			auto remit = std::remove_if(stages.begin(), stages.end(),
				[](const std::string& stage)
				{
					auto ext = fs::get_ext(stage);
			return ext == ".tesc" || ext == ".tese";
				});

			if (remit != stages.end())
				stages.erase(remit, stages.end());
		}
		else
			defineBlock << "#define USE_TESSELLATION" << '\n';

		auto api = graphics->getAPI();
		for (auto& stage : stages)
		{
			if (auto compiled = m_pCompiler->compile(stage, defineBlock.str(), api))
			{
				vShaderCode.insert(vShaderCode.end(), compiled->shaderCode.begin(), compiled->shaderCode.end());
				shaderCode.emplace_back(compiled);
			}
		}

		pipelineParams.alphaMode = specials.alphaBlend;
		pipelineParams.bindPoint = it->second.bindPoint;
		pipelineParams.cullMode = specials.doubleSided ? vk::CullModeFlagBits::eNone : it->second.cullMode;
		pipelineParams.depthTest = it->second.depthTest;
		pipelineParams.doubleSided = specials.doubleSided;
		pipelineParams.dynamicStates = it->second.dynamicStates;
		pipelineParams.enableTesselation = it->second.tesselation;
		pipelineParams.frontFace = it->second.frontFace;
		pipelineParams.primitiveTopology = it->second.topology;
		pipelineParams.renderStage = it->second.srStage;
		pipelineParams.useBindlessTextures = it->second.usesBindlessTextures;
		pipelineParams.vertexFree = it->second.vertexfree;
		pipelineParams.vertexType = it->second.vertexType;
		pipelineParams.subpass = specials.subpass;
		pipelineParams.usages = specials.usages;

		return shader_object_hash(vShaderCode, pipelineParams);
	}

	return {};
}

std::unique_ptr<CShaderObject> CShaderLoader::load(const std::vector<std::optional<FCachedShader>>& shaderCode, const FPipelineParams& pipelineParams)
{
	auto* graphics = m_pDevice->getAPI();

	auto pShaderObject = std::make_unique<CShaderObject>(m_pDevice);
	auto& pShader = pShaderObject->pShader;
	pShaderObject->pipelineParams = pipelineParams;

	for (auto& shader : shaderCode)
		pShader->addStage(shader->shaderCode, shader->shaderStage);

	pShader->buildReflection();

	pShaderObject->create(pipelineParams.subpass);
	pShaderObject->increaseUsage(pipelineParams.usages);

	return pShaderObject;
}