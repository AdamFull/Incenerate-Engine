#include "ShaderLoader.h"

#include "Engine.h"
#include "system/filesystem/filesystem.h"

constexpr const char* shader_config_path = "shaders.json";

using namespace engine::graphics;
using namespace engine::system;

CShaderLoader::CShaderLoader(CDevice* device)
{
	pDevice = device;

	load_config();
	pCompiler = std::make_unique<CShaderCompiler>();
}

CShaderLoader::~CShaderLoader()
{
	pCompiler = nullptr;
	pDevice = nullptr;
}

std::unique_ptr<CShaderObject> CShaderLoader::load(const std::string& name)
{
	auto it = programCI.find(name);
	if (it != programCI.end())
	{
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

void CShaderLoader::load_config()
{
	fs::read_json(shader_config_path, programCI);
}

void CShaderLoader::save_config()
{
	fs::write_json(shader_config_path, programCI);
}