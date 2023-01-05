#include "ShaderLoader.h"

#include "Engine.h"
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

void CShaderLoader::create(const std::string& srShaderPath)
{
	fs::read_json(srShaderPath, programCI);
}

std::unique_ptr<CShaderObject> CShaderLoader::load(const std::string& name, size_t mat_id)
{
	auto it = programCI.find(name);
	if (it != programCI.end())
	{
		if (mat_id != invalid_index)
		{
			auto& pMaterial = EGGraphics->getMaterial(mat_id);
			auto& params = pMaterial->getParameters();
			
			for (auto& definition : params.vCompileDefinitions)
				it->second.defines.emplace(definition, "");
		}

		auto api = pDevice->getAPI()->getAPI();

		auto pShaderObject = std::make_unique<CShaderObject>(pDevice);
		auto& pShader = pShaderObject->pShader;

		std::stringstream defineBlock;
		for (const auto& [defineName, defineValue] : it->second.defines)
			defineBlock << "#define " << defineName << " " << defineValue << '\n';

		for (auto& stage : it->second.stages)
		{
			if (auto compiled = pCompiler->compile(stage, defineBlock.str(), api))
				pShader->addStage(compiled->shaderCode, compiled->shaderStage);
		}
		pShader->buildReflection();

		pShaderObject->programCI = it->second;

		pShaderObject->create();

		return pShaderObject;
	}

	return nullptr;
}