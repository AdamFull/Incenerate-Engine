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

std::unique_ptr<CShaderObject> CShaderLoader::load(const std::string& name, size_t mat_id)
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

	return load(name, specials);
}

std::unique_ptr<CShaderObject> CShaderLoader::load(const std::string& name, const FShaderSpecials& specials)
{
	auto it = programCI.find(name);
	if (it != programCI.end())
	{
		std::vector<std::optional<FCachedShader>> vCompiledShaderStages{};
		std::vector<uint32_t> vShaderCode{};

		std::stringstream defineBlock;
		for(auto& [name, value] : specials.defines)
			defineBlock << "#define " << name << " " << value << '\n';

		for (const auto& [defineName, defineValue] : it->second.defines)
			defineBlock << "#define " << defineName << " " << defineValue << '\n';

		if (bEditorMode)
			defineBlock << "#define " << "EDITOR_MODE" << "\n";

		if(bBindlessFeature)
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

		auto* graphics = m_pDevice->getAPI();
		auto api = graphics->getAPI();
		for (auto& stage : stages)
		{
			if (auto compiled = m_pCompiler->compile(stage, defineBlock.str(), api))
			{
				vShaderCode.insert(vShaderCode.end(), compiled->shaderCode.begin(), compiled->shaderCode.end());
				vCompiledShaderStages.emplace_back(compiled);
			}
		}

		// Check here is shader already exists
		FPipelineParams pipelineParams{};
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

		auto pipeline_name = shader_object_hash(vShaderCode, pipelineParams);

		auto pShaderObject = std::make_unique<CShaderObject>(m_pDevice);
		auto& pShader = pShaderObject->pShader;

		for (auto& shader : vCompiledShaderStages)
			pShader->addStage(shader->shaderCode, shader->shaderStage);

		pShader->buildReflection();

		pShaderObject->pipelineParams = pipelineParams;

		if (graphics->getPipeline(pipeline_name))
		{
			pShaderObject->pipeline_id = graphics->getPipelineID(pipeline_name);
			graphics->incrementPipelineUsage(pShaderObject->pipeline_id);
		}
		else
		{
			std::unique_ptr<CPipeline> pPipeline{};
			switch (pipelineParams.bindPoint)
			{
			case vk::PipelineBindPoint::eGraphics: {
				pPipeline = std::make_unique<CGraphicsPipeline>(m_pDevice);
				pPipeline->create(pShaderObject.get(), graphics->getFramebuffer(pipelineParams.renderStage)->getRenderPass(), specials.subpass);
			} break;
			case vk::PipelineBindPoint::eCompute: {
				pPipeline = std::make_unique<CComputePipeline>(m_pDevice);
				pPipeline->create(pShaderObject.get());
			} break;
			}

			pShaderObject->pipeline_id = graphics->addPipeline(pipeline_name, std::move(pPipeline));
		}		

		pShaderObject->create(specials.usages);

		return pShaderObject;
	}

	return nullptr;
}