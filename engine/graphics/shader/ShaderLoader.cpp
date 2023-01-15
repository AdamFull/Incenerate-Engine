#include "ShaderLoader.h"

#include "Engine.h"
#include "graphics/rendering/material/Material.h"
#include "system/filesystem/filesystem.h"

constexpr const char* shader_config_path = "shaders.json";

using namespace engine::graphics;
using namespace engine::system;

CShaderLoader::CShaderLoader(CDevice* device)
{
	pDevice = device;
	pCompiler = std::make_unique<CShaderCompiler>();
}

CShaderLoader::~CShaderLoader()
{
	pCompiler = nullptr;
	pDevice = nullptr;
}

void CShaderLoader::create()
{
	fs::read_json(shader_config_path, programCI, true);
}

std::unique_ptr<CShaderObject> CShaderLoader::load(const std::string& name, size_t mat_id)
{
	FShaderSpecials specials;

	if (mat_id != invalid_index)
	{
		auto& pMaterial = EGGraphics->getMaterial(mat_id);
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
		std::stringstream defineBlock;
		for(auto& [name, value] : specials.defines)
			defineBlock << "#define " << name << " " << value << '\n';

		auto pShaderObject = std::make_unique<CShaderObject>(pDevice);
		pShaderObject->_alphaMode = specials.alphaBlend;
		auto& pShader = pShaderObject->pShader;

		for (const auto& [defineName, defineValue] : it->second.defines)
			defineBlock << "#define " << defineName << " " << defineValue << '\n';


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

		auto api = EGGraphics->getAPI();
		for (auto& stage : stages)
		{
			if (auto compiled = pCompiler->compile(stage, defineBlock.str(), api))
				pShader->addStage(compiled->shaderCode, compiled->shaderStage);
		}

		pShader->buildReflection();

		pShaderObject->programCI = it->second;

		pShaderObject->create(specials.subpass, specials.usages);

		return pShaderObject;
	}

	return nullptr;
}