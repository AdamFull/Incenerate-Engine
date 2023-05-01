#include "ShaderLoader.h"

#include "Engine.h"
#include "graphics/rendering/material/Material.h"
#include "filesystem/vfs_helper.h"

constexpr const char* shader_config_path = "/embed/shaders.json";

using namespace engine::graphics;
using namespace engine::filesystem;

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
	EGFilesystem->readJson(shader_config_path, programCI);
}

std::unique_ptr<CShaderObject> CShaderLoader::load(const std::string& name, size_t mat_id)
{
	auto* graphics = pDevice->getAPI();
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
		std::stringstream defineBlock;
		for(auto& [name, value] : specials.defines)
			defineBlock << "#define " << name << " " << value << '\n';

		auto pShaderObject = std::make_unique<CShaderObject>(pDevice);
		pShaderObject->_alphaMode = specials.alphaBlend;
		auto& pShader = pShaderObject->pShader;

		for (const auto& [defineName, defineValue] : it->second.defines)
			defineBlock << "#define " << defineName << " " << defineValue << '\n';

		if (EGEngine->isEditorMode())
			defineBlock << "#define " << "EDITOR_MODE" << "\n";

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

		auto* graphics = pDevice->getAPI();
		auto api = graphics->getAPI();
		for (auto& stage : stages)
		{
			if (auto compiled = pCompiler->compile(stage, defineBlock.str(), api))
				pShader->addStage(compiled->shaderCode, compiled->shaderStage);
		}

		pShader->buildReflection();

		pShaderObject->programCI = it->second;
		pShaderObject->programCI.cullMode = specials.doubleSided ? vk::CullModeFlagBits::eNone : pShaderObject->programCI.cullMode;

		pShaderObject->create(specials.subpass, specials.usages);

		return pShaderObject;
	}

	return nullptr;
}